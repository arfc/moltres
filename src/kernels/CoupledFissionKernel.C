#include "CoupledFissionKernel.h"

template<>
InputParameters validParams<CoupledFissionKernel>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("temperature", "The temperature used to interpolate material properties");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  params.addRequiredParam<bool>("account_delayed", "Whether to account for delayed neutrons.");
  return params;
}

CoupledFissionKernel::CoupledFissionKernel(const InputParameters & parameters) :
    Kernel(parameters),
    ScalarTransportBase(parameters),
    _nsf(getMaterialProperty<std::vector<Real> >("nsf")),
    _d_nsf_d_temp(getMaterialProperty<std::vector<Real> >("d_nsf_d_temp")),
    _chi(getMaterialProperty<std::vector<Real> >("chi")),
    _d_chi_d_temp(getMaterialProperty<std::vector<Real> >("d_chi_d_temp")),
    _beta(getMaterialProperty<Real>("beta")),
    _d_beta_d_temp(getMaterialProperty<Real>("d_beta_d_temp")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
    _temp_id(coupled("temperature")),
    _temp(coupledValue("temperature")),
    _account_delayed(getParam<bool>("account_delayed"))
{
  unsigned int n = coupledComponents("group_fluxes");
  if (!(n == _num_groups))
  {
    mooseError("The number of coupled variables doesn't match the number of groups.");
  }
  _group_fluxes.resize(n);
  _flux_ids.resize(n);
  for (unsigned int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _flux_ids[i] = coupled("group_fluxes", i);
  }
}

Real
CoupledFissionKernel::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
    r += -_nsf[_qp][i] * computeConcentration((*_group_fluxes[i]), _qp);

  if (_account_delayed)
    r *= (1. - _beta[_qp]);

  return _test[_i][_qp] * _chi[_qp][_group] * r;
}

Real
CoupledFissionKernel::computeQpJacobian()
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (i == _group)
    {
      jac = -_nsf[_qp][i] * computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp);
      break;
    }
  }

  if (_account_delayed)
    jac *= (1. - _beta[_qp]);

  return _test[_i][_qp] * _chi[_qp][_group] * jac;
}

Real
CoupledFissionKernel::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i])
    {
      jac = -_test[_i][_qp] * _chi[_qp][_group] * _nsf[_qp][i] * computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp);
      if (_account_delayed)
        jac *= (1. - _beta[_qp]);
      break;
    }
  }

  if (jvar == _temp_id)
  {
    for (unsigned int i = 0; i < _num_groups; ++i)
    {
      if (_account_delayed)
        jac += -_test[_i][_qp] * computeConcentration((*_group_fluxes[i]), _qp) * (_d_chi_d_temp[_qp][_group] * _phi[_j][_qp] * _nsf[_qp][i] * (1. - _beta[_qp]) + _chi[_qp][_group] * _d_nsf_d_temp[_qp][i] * _phi[_j][_qp] * (1. - _beta[_qp]) + _chi[_qp][_group] * _nsf[_qp][i] * -_d_beta_d_temp[_qp] * _phi[_j][_qp]);
      else
        jac += -_test[_i][_qp] * computeConcentration((*_group_fluxes[i]), _qp) * (_d_chi_d_temp[_qp][_group] * _phi[_j][_qp] * _nsf[_qp][i] + _chi[_qp][_group] * _d_nsf_d_temp[_qp][i] * _phi[_j][_qp]);
    }
  }

  return jac;
}
