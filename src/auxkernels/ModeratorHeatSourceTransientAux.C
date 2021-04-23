#include "ModeratorHeatSourceTransientAux.h"
#include "Function.h"

InputParameters
ModeratorHeatSourceTransientAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addRequiredParam<PostprocessorName>(
      "average_fission_heat",
      "The average fission heat being generated in the fuel portion of the reactor.");
  params.addRequiredParam<FunctionName>(
      "gamma", "The ratio of power density generated in the moderator vs. the fuel.");
  return params;
}

ModeratorHeatSourceTransientAux::ModeratorHeatSourceTransientAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _average_fission_heat(getPostprocessorValue("average_fission_heat")),
    _gamma(getFunction("gamma"))
{
}

Real
ModeratorHeatSourceTransientAux::computeValue()
{
  return _average_fission_heat * _gamma.value(_t, _q_point[_qp]);
}
