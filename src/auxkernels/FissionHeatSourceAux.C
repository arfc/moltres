#include "FissionHeatSourceAux.h"

template<>
InputParameters validParams<FissionHeatSourceAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  params.addRequiredParam<PostprocessorName>("tot_fissions", "The total fission postprocessor that's used to normalize the heat source.");
  params.addRequiredParam<Real>("power", "The reactor power.");
  return params;
}

FissionHeatSourceAux::FissionHeatSourceAux(const InputParameters & parameters) :
    AuxKernel(parameters),
    _fissxs(getMaterialProperty<std::vector<Real> >("fissxs")),
    _d_fissxs_d_temp(getMaterialProperty<std::vector<Real> >("d_fissxs_d_temp")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _tot_fissions(getPostprocessorValue("tot_fissions")),
    _power(getParam<Real>("power"))
{
  auto n = coupledComponents("group_fluxes");
  if (!(n == _num_groups))
  {
    mooseError("The number of coupled variables doesn't match the number of groups.");
  }
  _group_fluxes.resize(n);
  _flux_ids.resize(n);
  for (decltype(n) i = 0; i < n; ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _flux_ids[i] = coupled("group_fluxes", i);
  }
}

Real
FissionHeatSourceAux::computeValue()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    r += _fissxs[_qp][i] * (*_group_fluxes[i])[_qp] * _power / _tot_fissions;
  }

  return r;
}
