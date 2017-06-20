#include "ModeratorHeatSourceTransientAux.h"

template <>
InputParameters
validParams<ModeratorHeatSourceTransientAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<PostprocessorName>(
      "average_fission_heat",
      "The average fission heat being generated in the fuel portion of the reactor.");
  params.addRequiredParam<Real>(
      "gamma", "The ratio of power density generated in the moderator vs. the fuel.");
  return params;
}

ModeratorHeatSourceTransientAux::ModeratorHeatSourceTransientAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _average_fission_heat(getPostprocessorValue("average_fission_heat")),
    _gamma(getParam<Real>("gamma"))
{
}

Real
ModeratorHeatSourceTransientAux::computeValue()
{
  return _average_fission_heat * _gamma;
}
