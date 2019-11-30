#include "TransientFissionHeatSource.h"

registerMooseObject("MoltresApp", TransientFissionHeatSource);

template <>
InputParameters
validParams<TransientFissionHeatSource>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. "
                                               "These MUST be listed by decreasing "
                                               "energy/increasing group number.");
  params.addParam<Real>("nt_scale", 1, "Scaling of the neutron fluxes to aid convergence.");
  params.addParam<unsigned int>("num_decay_heat_groups", 0, "The number of decay heat groups.");
  params.addCoupledVar("heat_concs", "All the variables that hold the decay heat "
                                     "precursor concentrations.");
  params.addParam<std::vector<Real>>("decay_heat_fractions", "Decay Heat Fractions");
  params.addParam<std::vector<Real>>("decay_heat_constants", "Decay Heat Constants");
  params.addParam<bool>("account_decay_heat", false, "Whether to account for decay heat.");
  return params;
}

TransientFissionHeatSource::TransientFissionHeatSource(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _fissxs(getMaterialProperty<std::vector<Real>>("fissxs")),
    _d_fissxs_d_temp(getMaterialProperty<std::vector<Real>>("d_fissxs_d_temp")),
    _fisse(getMaterialProperty<std::vector<Real>>("fisse")),
    _d_fisse_d_temp(getMaterialProperty<std::vector<Real>>("d_fisse_d_temp")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _nt_scale(getParam<Real>("nt_scale")),
    _account_decay_heat(getParam<bool>("account_decay_heat")),
    _num_heat_groups(getParam<unsigned int>("num_decay_heat_groups")),
    _decay_heat_frac(getParam<std::vector<Real>>("decay_heat_fractions")),
    _decay_heat_const(getParam<std::vector<Real>>("decay_heat_constants"))
{
  _group_fluxes.resize(_num_groups);
  _flux_ids.resize(_num_groups);
  for (unsigned int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _flux_ids[i] = coupled("group_fluxes", i);
  }
  if (_account_decay_heat)
  {
    unsigned int n = coupledComponents("heat_concs");
    if (!(n == _num_heat_groups))
    {
      mooseError("The number of coupled variables doesn't match the number of decay heat groups.");
    }
    _heat_concs.resize(n);
    _heat_ids.resize(n);
    for (unsigned int i = 0; i < _heat_concs.size(); ++i)
    {
      _heat_concs[i] = &coupledValue("heat_concs", i);
      _heat_ids[i] = coupled("heat_concs", i);
    }
  }
}

Real
TransientFissionHeatSource::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    r += -_test[_i][_qp] * _fisse[_qp][i] * _fissxs[_qp][i] *
         computeConcentration((*_group_fluxes[i]), _qp) * _nt_scale;
  }

  Real frac = 0;
  for (unsigned int i = 0; i < _num_heat_groups; ++i)
  {
    frac += _decay_heat_frac[i];
  }

  if (_account_decay_heat)
  {
    r *= (1. - frac);
  }

  return r;
}

Real
TransientFissionHeatSource::computeQpJacobian()
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    jac += -_test[_i][_qp] * (_fisse[_qp][i] * _d_fissxs_d_temp[_qp][i] * _phi[_j][_qp] +
                              _d_fisse_d_temp[_qp][i] * _phi[_j][_qp] * _fissxs[_qp][i]) *
           computeConcentration((*_group_fluxes[i]), _qp) * _nt_scale;
  }

  Real frac = 0;
  for (unsigned int i = 0; i < _num_heat_groups; ++i)
  {
    frac += _decay_heat_frac[i];
  }

  if (_account_decay_heat)
  {
    jac *= (1. - frac);
  }

  return jac;
}

Real
TransientFissionHeatSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i])
    {
      jac += -_test[_i][_qp] * _fisse[_qp][i] * _fissxs[_qp][i] *
             computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp) * _nt_scale;
      
      Real frac = 0;
      for (unsigned int i = 0; i < _num_heat_groups; ++i)
      {
        frac += _decay_heat_frac[i];
      }

      if (_account_decay_heat)
        jac *= (1. - frac);
      break;
    }
  }

  return jac;
}
