#include "SNFission.h"

#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNFission);

InputParameters
SNFission::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups");
  params.addRequiredCoupledVar("group_angular_fluxes",
      "All the variables that hold the group angular fluxes. These MUST be listed by decreasing "
      "energy/increasing group number.");
  params.addRequiredParam<bool>("account_delayed", "Whether to account for delayed neutrons.");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}

SNFission::SNFission(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _nsf(getMaterialProperty<std::vector<Real>>("nsf")),
    _d_nsf_d_temp(getMaterialProperty<std::vector<Real>>("d_nsf_d_temp")),
    _chi_t(getMaterialProperty<std::vector<Real>>("chi_t")),
    _d_chi_t_d_temp(getMaterialProperty<std::vector<Real>>("d_chi_t_d_temp")),
    _chi_p(getMaterialProperty<std::vector<Real>>("chi_p")),
    _d_chi_p_d_temp(getMaterialProperty<std::vector<Real>>("d_chi_p_d_temp")),
    _beta(getMaterialProperty<Real>("beta")),
    _d_beta_d_temp(getMaterialProperty<Real>("d_beta_d_temp")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
    _temp_id(coupled("temperature")),
    _account_delayed(getParam<bool>("account_delayed"))
{
  unsigned int n = coupledComponents("group_angular_fluxes");
  if (n != _num_groups)
    mooseError("The number of coupled variables doesn't match the number of groups.");
  _group_fluxes.resize(n);
  _flux_ids.resize(n);
  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    _group_fluxes[g] = &coupledArrayValue("group_angular_fluxes", g);
    _flux_ids[g] = coupled("group_angular_fluxes", g);
  }

  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNFission::computeQpResidual(RealEigenVector & residual)
{
  if (_chi_p[_qp][_group] == 0. && _chi_t[_qp][_group] == 0.)
    return;

  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);

  Real fission = 0.;
  for (unsigned int g = 0; g < _num_groups; ++g)
    fission += .125 * _nsf[_qp][g] * _weights.transpose() * (*_group_fluxes[g])[_qp];

  residual = -_weights.cwiseProduct(lhs) * fission;

  if (_account_delayed)
    residual *= (1. - _beta[_qp]) * _chi_p[_qp][_group];
  else
    residual *= _chi_t[_qp][_group];
}

RealEigenVector
SNFission::computeQpJacobian()
{
  if (_chi_p[_qp][_group] == 0. && _chi_t[_qp][_group] == 0.)
    return ArrayKernel::computeQpJacobian();

  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);

  RealEigenVector jac = -_nsf[_qp][_group] * _phi[_j][_qp] * _weights / 8.;

  if (_account_delayed)
    jac *= (1. - _beta[_qp]) * _chi_p[_qp][_group];
  else
    jac *= _chi_t[_qp][_group];

  return _weights.cwiseProduct(lhs).cwiseProduct(jac);
}

RealEigenMatrix
SNFission::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  if (_chi_p[_qp][_group] == 0. && _chi_t[_qp][_group] == 0.)
    return ArrayKernel::computeQpOffDiagJacobian(jvar);

  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);

  for (unsigned int g = 0; g < _num_groups; ++g)
    if (jvar.number() == _flux_ids[g])
    {
      RealEigenMatrix jac = RealEigenMatrix::Zero(_count, _count);

      for (unsigned int i = 0; i < _count; ++i)
        for (unsigned int j = 0; j < _count; ++j)
          jac(i, j) -= _weights(i) * lhs(i) * _nsf[_qp][g] * _phi[_j][_qp] * _weights(j) / 8.;

      if (_account_delayed)
        jac *= (1. - _beta[_qp]) * _chi_p[_qp][_group];
      else
        jac *= _chi_t[_qp][_group];

      return jac;
    }
  return ArrayKernel::computeQpOffDiagJacobian(jvar);
}
