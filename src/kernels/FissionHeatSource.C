#include "FissionHeatSource.h"

registerMooseObject("MoltresApp", FissionHeatSource);

InputParameters
FissionHeatSource::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. "
                                               "These MUST be listed by decreasing "
                                               "energy/increasing group number.");
  params.addCoupledVar("heat_source",
                       "Heat source variable name. Optional parameter for reducing variable data "
                       "transfers between MultiApps with a consolidated heat source variable.");
  params.addRequiredParam<PostprocessorName>(
      "tot_fission_heat", "The total fission heat postprocessor that's used to normalize the heat source.");
  params.addRequiredParam<Real>("power", "The reactor power.");
  params.addParam<Real>("gamma_frac", 0.0,
      "Fraction of total fission heat generated in non-fuel regions due to gamma heating");
  return params;
}

FissionHeatSource::FissionHeatSource(const InputParameters & parameters)
  : Kernel(parameters),
    _fissxs(getMaterialProperty<std::vector<Real>>("fissxs")),
    _d_fissxs_d_temp(getMaterialProperty<std::vector<Real>>("d_fissxs_d_temp")),
    _fisse(getMaterialProperty<std::vector<Real>>("fisse")),
    _d_fisse_d_temp(getMaterialProperty<std::vector<Real>>("d_fisse_d_temp")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _tot_fission_heat(getPostprocessorValue("tot_fission_heat")),
    _power(getParam<Real>("power")),
    _has_heat_source(isCoupled("heat_source")),
    _heat_source(isCoupled("heat_source") ? coupledValue("heat_source") : _zero),
    _gamma_frac(getParam<Real>("gamma_frac"))
{
  if (!_has_heat_source)
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
}

Real
FissionHeatSource::computeQpResidual()
{
  Real r = 0;
  if (_has_heat_source)
    r += -_test[_i][_qp] * _heat_source[_qp] * _power / _tot_fission_heat;
  else
    for (unsigned int i = 0; i < _num_groups; ++i)
      r += -_test[_i][_qp] * _fisse[_qp][i] * _fissxs[_qp][i] * (*_group_fluxes[i])[_qp] * _power /
           _tot_fission_heat;

  r *= (1.0 - _gamma_frac);

  return r;
}

Real
FissionHeatSource::computeQpJacobian()
{
  if (_has_heat_source)
    return 0.0;
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    jac += -_test[_i][_qp] * (_fisse[_qp][i] * _d_fissxs_d_temp[_qp][i] * _phi[_j][_qp] +
                              _d_fisse_d_temp[_qp][i] * _phi[_j][_qp] * _fissxs[_qp][i]) *
                              (*_group_fluxes[i])[_qp] * _power / _tot_fission_heat;

  }

  return jac;
}

Real
FissionHeatSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (_has_heat_source)
    return 0.0;
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i])
    {
      jac += -_test[_i][_qp] * _fisse[_qp][i] * _fissxs[_qp][i] * _phi[_j][_qp] *
             _power / _tot_fission_heat;
      break;
    }
  }

  return jac;
}
