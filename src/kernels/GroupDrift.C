#include "GroupDrift.h"

#include "MoltresUtils.h"

registerMooseObject("MoltresApp", GroupDrift);

InputParameters
GroupDrift::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number",
                                        "The group for which this kernel controls diffusion");
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups");
  params.addRequiredCoupledVar("group_angular_fluxes",
      "All the variables that hold the group angular fluxes. These MUST be listed by decreasing "
      "energy/increasing group number.");
  params.addCoupledVar("temperature",
                       "The temperature used to interpolate the diffusion coefficient");
  return params;
}

GroupDrift::GroupDrift(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _diffcoef(getMaterialProperty<std::vector<Real>>("diffcoef")),
    _d_diffcoef_d_temp(getMaterialProperty<std::vector<Real>>("d_diffcoef_d_temp")),
    _totxs(getMaterialProperty<std::vector<Real>>("totxs")),
    _d_totxs_d_temp(getMaterialProperty<std::vector<Real>>("d_totxs_d_temp")),
    _scatter(getMaterialProperty<std::vector<Real>>("scatter")),
    _d_scatter_d_temp(getMaterialProperty<std::vector<Real>>("d_scatter_d_temp")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
    _temp_id(coupled("temperature"))
{
  unsigned int n = coupledComponents("group_angular_fluxes");
  if (n != _num_groups)
    mooseError("The number of coupled variables doesn't match the number of groups.");
  _group_fluxes.resize(n);
  _grad_group_fluxes.resize(n);
  _flux_ids.resize(n);
  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    _group_fluxes[g] = &coupledArrayValue("group_angular_fluxes", g);
    _grad_group_fluxes[g] = &coupledArrayGradient("group_angular_fluxes", g);
    _flux_ids[g] = coupled("group_angular_fluxes", g);
  }

  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

Real
GroupDrift::computeQpResidual()
{
  Real denom = _weights.transpose() * (*_group_fluxes[_group])[_qp];
  RealEigenVector D = RealEigenVector::Zero(3);
  for (unsigned int i = 0; i < (*_group_fluxes[_group])[_qp].size(); ++i)
  {
    D += _weights(i) * _tau_sn[_qp][_group] * (_ordinates.row(i).transpose() * _ordinates.row(i)) *
      (*_grad_group_fluxes[_group])[_qp].row(i).transpose();
  }
  D += (_tau_sn[_qp][_group] * _totxs[_qp][_group] - 1) * _ordinates.transpose() *
    _weights.cwiseProduct((*_group_fluxes[_group])[_qp]) -
    _diffcoef[_qp][_group] * (*_grad_group_fluxes[_group])[_qp].transpose() * _weights;
  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    unsigned int scatter_idx = _num_groups * _num_groups + g * _num_groups + _group;
    if (_scatter[_qp][scatter_idx] == 0.)
      continue;
    D -= _tau_sn[_qp][_group] * _scatter[_qp][scatter_idx] * _ordinates.transpose() *
      _weights.cwiseProduct((*_group_fluxes[g])[_qp]);
  }
  D /= (denom + 1e-9);
  RealEigenVector array_grad_test(3);
  array_grad_test << _grad_test[_i][_qp](0),
                     _grad_test[_i][_qp](1),
                     _grad_test[_i][_qp](2);
  return (computeConcentration(_u, _qp) * array_grad_test * D)(0);
}

Real
GroupDrift::computeQpJacobian()
{
  Real denom = _weights.transpose() * (*_group_fluxes[_group])[_qp];
  RealEigenVector D = RealEigenVector::Zero(3);
  for (unsigned int i = 0; i < (*_group_fluxes[_group])[_qp].size(); ++i)
  {
    D += _weights(i) * _tau_sn[_qp][_group] * (_ordinates.row(i).transpose() * _ordinates.row(i)) *
      (*_grad_group_fluxes[_group])[_qp].row(i).transpose();
  }
  D += (_tau_sn[_qp][_group] * _totxs[_qp][_group] - 1) * _ordinates.transpose() *
    _weights.cwiseProduct((*_group_fluxes[_group])[_qp]) -
    _diffcoef[_qp][_group] * (*_grad_group_fluxes[_group])[_qp].transpose() * _weights;
  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    unsigned int scatter_idx = _num_groups * _num_groups + g * _num_groups + g;
    if (_scatter[_qp][scatter_idx] == 0.)
      continue;
    D -= _tau_sn[_qp][_group] * _scatter[_qp][scatter_idx] * _ordinates.transpose() *
      _weights.cwiseProduct((*_group_fluxes[g])[_qp]);
  }
  D /= (denom + 1e-9);
  RealEigenVector array_grad_test(3);
  array_grad_test << _grad_test[_i][_qp](0),
                     _grad_test[_i][_qp](1),
                     _grad_test[_i][_qp](2);
  return (computeConcentrationDerivative(_u, _phi, _j, _qp) * array_grad_test * D)(0);
}

Real
GroupDrift::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _temp_id)
    return _d_diffcoef_d_temp[_qp][_group] * _phi[_j][_qp] * _grad_test[_i][_qp] *
           computeConcentrationGradient(_u, _grad_u, _qp);
  else
    return 0;
}
