#include "FissionHeatSourceAux.h"

registerMooseObject("MoltresApp", FissionHeatSourceAux);

InputParameters
FissionHeatSourceAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. "
                                               "These MUST be listed by decreasing "
                                               "energy/increasing group number.");
  params.addRequiredParam<PostprocessorName>(
      "tot_fission_heat", "The total fission heat postprocessor that's used to normalize the heat source.");
  params.addRequiredParam<Real>("power", "The reactor power.");
  return params;
}

FissionHeatSourceAux::FissionHeatSourceAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _fissxs(getMaterialProperty<std::vector<Real>>("fissxs")),
    _fisse(getMaterialProperty<std::vector<Real>>("fisse")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _tot_fission_heat(getPostprocessorValue("tot_fission_heat")),
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
    r += _fisse[_qp][i] * _fissxs[_qp][i] * (*_group_fluxes[i])[_qp] * _power / _tot_fission_heat;
  }

  return r;
}
