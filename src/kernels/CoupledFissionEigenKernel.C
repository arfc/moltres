#include "CoupledFissionEigenKernel.h"

template<>
InputParameters validParams<CoupledFissionEigenKernel>()
{
  InputParameters params = validParams<EigenKernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  params.addRequiredParam<bool>("account_delayed", "Whether to account for delayed neutrons.");
  return params;
}

CoupledFissionEigenKernel::CoupledFissionEigenKernel(const InputParameters & parameters) :
    EigenKernel(parameters),
    ScalarTransportBase(parameters),
    _nsf(getMaterialProperty<std::vector<Real> >("nsf")),
    _chi(getMaterialProperty<std::vector<Real> >("chi")),
    _beta(getMaterialProperty<Real>("beta")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
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
CoupledFissionEigenKernel::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
    r += -_test[_i][_qp] * _chi[_qp][_group] * _nsf[_qp][i] * computeConcentration((*_group_fluxes[i]), _qp);

  if (_account_delayed)
    r *= (1. - _beta[_qp]);

  return r;
}

Real
CoupledFissionEigenKernel::computeQpJacobian()
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (i == _group)
    {
      jac += -_test[_i][_qp] * _chi[_qp][_group] * _nsf[_qp][i] * computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp);
      if (_account_delayed)
        jac *= (1. - _beta[_qp]);
      break;
    }
  }

  return jac;
}

Real
CoupledFissionEigenKernel::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i])
    {
      jac += -_test[_i][_qp] * _chi[_qp][_group] * _nsf[_qp][i] * computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp);
      if (_account_delayed)
        jac *= (1. - _beta[_qp]);
      break;
    }
  }

  return jac;
}
