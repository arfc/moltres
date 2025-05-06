#include "GroupDriftAux.h"

#include "MoltresUtils.h"
using MooseUtils::relativeFuzzyEqual;

registerMooseObject("MoltresApp", GroupDriftAux);

InputParameters
GroupDriftAux::validParams()
{
  InputParameters params = ArrayAuxKernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number",
                                        "The group for which this kernel controls diffusion");
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups");
  params.addRequiredCoupledVar("group_angular_fluxes",
      "All the variables that hold the group angular fluxes. These MUST be listed by decreasing "
      "energy/increasing group number.");
  params.addParam<bool>("set_diffcoef_limit",
      false,
      "Replaces all diffusion coefficient values above the specified limit to the limit value. "
      "Primarily helps with stabilizing drift coefficients in void regions.");
  params.addParam<Real>("diffcoef_limit", 5.0, "Maximum diffusion coefficient value limit.");
  return params;
}

GroupDriftAux::GroupDriftAux(const InputParameters & parameters)
  : ArrayAuxKernel(parameters),
    ScalarTransportBase(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _diffcoef(getMaterialProperty<std::vector<Real>>("diffcoef")),
    _totxs(getMaterialProperty<std::vector<Real>>("totxs")),
    _scatter(getMaterialProperty<std::vector<Real>>("scatter")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
    _set_limit(getParam<bool>("set_diffcoef_limit")),
    _limit(getParam<Real>("diffcoef_limit"))
{
  if (_var.count() != 3)
    mooseError("The number of group drift array variables must be 3.");
  unsigned int n = coupledComponents("group_angular_fluxes");
  if (n != _num_groups)
    mooseError("The number of coupled variables doesn't match the number of groups.");
  _group_fluxes.resize(n);
  _grad_group_fluxes.resize(n);
  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    _group_fluxes[g] = &coupledArrayValue("group_angular_fluxes", g);
    _grad_group_fluxes[g] = &coupledArrayGradient("group_angular_fluxes", g);
  }

  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

RealEigenVector
GroupDriftAux::computeValue()
{
  Real diffcoef;
  if (_set_limit && _diffcoef[_qp][_group] > _limit)
    diffcoef = _limit;
  else
    diffcoef = _diffcoef[_qp][_group];

  Real denom = _weights.transpose() * (*_group_fluxes[_group])[_qp];
  RealEigenVector D = RealEigenVector::Zero(3);
  if (relativeFuzzyEqual(denom, 0.0) || denom < 0.0)
    return D;
  // Eddington term: \sum_d (w_d \tau_g \Omega_d \Omega_d \cdot \grad\Psi_{g,d})
  for (unsigned int i = 0; i < (*_group_fluxes[_group])[_qp].size(); ++i)
  {
    D += _weights(i) * _tau_sn[_qp][_group] * (_ordinates.row(i).transpose() * _ordinates.row(i)) *
      (*_grad_group_fluxes[_group])[_qp].row(i).transpose();
  }
  // Collision, current, & diffusion terms:
  // \sum_d (w_d (tau_g \Sigma_{t,g} - 1) \Omega_d \Psi_{g,d} - D\nabla\Psi_{g,d})
  D += (_tau_sn[_qp][_group] * _totxs[_qp][_group] - 1) * _ordinates.transpose() *
    _weights.cwiseProduct((*_group_fluxes[_group])[_qp]) -
    diffcoef * (*_grad_group_fluxes[_group])[_qp].transpose() * _weights;
  // 1st-order scattering term: -\sum_d (w_d tau_g S_1 \Omega_d\Psi_{g',d})
  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    unsigned int scatter_idx = _num_groups * _num_groups + g * _num_groups + _group;
    if (relativeFuzzyEqual(_scatter[_qp][scatter_idx], 0.0))
      continue;
    D -= _tau_sn[_qp][_group] * _scatter[_qp][scatter_idx] * _ordinates.transpose() *
      _weights.cwiseProduct((*_group_fluxes[g])[_qp]);
  }
  // Normalized by scalar flux
  D /= denom;
  // Avoid excessively high drift values to maintain solver stability in coarse meshes.
  // This occurs when there are very small fluxes near void and control rod regions.
  for (unsigned int i = 0; i < 3; ++i)
  {
    Real abs_val = std::abs(D(i));
    if (_totxs[_qp][_group] < 1e-2 && abs_val > 2.) // void regions
      D(i) *= 2. / abs_val;
    else if (abs_val > 1.) // other regions
      D(i) *= 1. / abs_val;
  }
  return D;
}
