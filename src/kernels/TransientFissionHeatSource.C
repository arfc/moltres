#include "TransientFissionHeatSource.h"

template<>
InputParameters validParams<TransientFissionHeatSource>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  params.addParam<Real>("nt_scale", 1, "Scaling of the neutron fluxes to aid convergence.");
  return params;
}

TransientFissionHeatSource::TransientFissionHeatSource(const InputParameters & parameters) :
    Kernel(parameters),
    ScalarTransportBase(parameters),
    _fissxs(getMaterialProperty<std::vector<Real> >("fissxs")),
    _d_fissxs_d_temp(getMaterialProperty<std::vector<Real> >("d_fissxs_d_temp")),
    _fisse(getMaterialProperty<std::vector<Real> >("fisse")),
    _d_fisse_d_temp(getMaterialProperty<std::vector<Real> >("d_fisse_d_temp")),
    _num_groups(getParam<int>("num_groups")),
    _nt_scale(getParam<Real>("nt_scale"))
{
  int n = coupledComponents("group_fluxes");
  if (!(n == _num_groups))
  {
    mooseError("The number of coupled variables doesn't match the number of groups.");
  }
  _group_fluxes.resize(n);
  _flux_ids.resize(n);
  for (int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _flux_ids[i] = coupled("group_fluxes", i);
  }
}

Real
TransientFissionHeatSource::computeQpResidual()
{
  Real r = 0;
  for (int i = 0; i < _num_groups; ++i)
  {
    r += -_test[_i][_qp] * _fisse[_qp][i] * _fissxs[_qp][i] * computeConcentration((*_group_fluxes[i]), _qp) * _nt_scale;
  }

  return r;
}

Real
TransientFissionHeatSource::computeQpJacobian()
{
  Real jac = 0;
  for (int i = 0; i < _num_groups; ++i)
  {
    jac += -_test[_i][_qp] * (_fisse[_qp][i] * _d_fissxs_d_temp[_qp][i] * _phi[_j][_qp] + _d_fisse_d_temp[_qp][i] * _phi[_j][_qp] * _fissxs[_qp][i]) * computeConcentration((*_group_fluxes[i]), _qp) * _nt_scale;
  }

  return jac;
}

Real
TransientFissionHeatSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i])
    {
      jac += -_test[_i][_qp] * _fisse[_qp][i] * _fissxs[_qp][i] * computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp) * _nt_scale;
      break;
    }
  }

  return jac;
}
