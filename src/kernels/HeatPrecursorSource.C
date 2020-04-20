#include "HeatPrecursorSource.h"

registerMooseObject("MoltresApp", HeatPrecursorSource);

template <>
InputParameters
validParams<HeatPrecursorSource>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups");
  params.addRequiredParam<unsigned int>("decay_heat_group_number",
                                        "What decay heat group this kernel is acting on.");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. "
                                               "These MUST be listed by decreasing "
                                               "energy/increasing group number.");
  params.addParam<Real>("nt_scale", 1, "Scaling of the neutron fluxes to aid convergence.");
  params.addCoupledVar(
      "temperature", 800, "The temperature used to interpolate material properties.");
  params.addRequiredParam<std::vector<Real>>("decay_heat_fractions", "Decay Heat Fractions");
  params.addRequiredParam<std::vector<Real>>("decay_heat_constants", "Decay Heat Constants");
  return params;
}

HeatPrecursorSource::HeatPrecursorSource(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _fisse(getMaterialProperty<std::vector<Real>>("fisse")),
    _d_fisse_d_temp(getMaterialProperty<std::vector<Real>>("d_fisse_d_temp")),
    _fissxs(getMaterialProperty<std::vector<Real>>("fissxs")),
    _d_fissxs_d_temp(getMaterialProperty<std::vector<Real>>("d_fissxs_d_temp")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _heat_group(getParam<unsigned int>("decay_heat_group_number") - 1),
    _nt_scale(getParam<Real>("nt_scale")),
    _decay_heat_frac(getParam<std::vector<Real>>("decay_heat_fractions")),
    _decay_heat_const(getParam<std::vector<Real>>("decay_heat_constants")),
    _temp(coupledValue("temperature")),
    _temp_id(coupled("temperature"))
{
  _group_fluxes.resize(_num_groups);
  _flux_ids.resize(_num_groups);
  for (unsigned int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _flux_ids[i] = coupled("group_fluxes", i);
  }
}

Real
HeatPrecursorSource::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    r += -_test[_i][_qp] * _decay_heat_frac[_heat_group] *
         _decay_heat_const[_heat_group] * _fisse[_qp][i] * _fissxs[_qp][i] * 
         computeConcentration((*_group_fluxes[i]), _qp) * _nt_scale;
  }

  return r;
}

Real
HeatPrecursorSource::computeQpJacobian()
{
  return 0;
}

Real
HeatPrecursorSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
    if (jvar == _flux_ids[i])
    {
      jac = -_test[_i][_qp] * _decay_heat_frac[_heat_group] *
            _decay_heat_const[_heat_group] * _fisse[_qp][i] * _fissxs[_qp][i] * 
            computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp) * _nt_scale;
      return jac;
    }

  if (jvar == _temp_id)
  {
    for (unsigned int i = 0; i < _num_groups; ++i)
      jac += -_test[_i][_qp] * _decay_heat_frac[_heat_group] *
             _decay_heat_const[_heat_group] *
             (_fisse[_qp][i] * _d_fissxs_d_temp[_qp][i] * _phi[_j][_qp] *
                    computeConcentration((*_group_fluxes[i]), _qp) +
              _d_fisse_d_temp[_qp][i] * _fissxs[_qp][i] * _phi[_j][_qp] *
                    computeConcentration((*_group_fluxes[i]), _qp)) * _nt_scale;
    return jac;
  }

  return 0;
}
