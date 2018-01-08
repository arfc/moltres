#include "GammaHeatSource.h"
#include "Function.h"

template <>
InputParameters
validParams<GammaHeatSource>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<PostprocessorName>(
      "average_fission_heat",
      "The average fission heat being generated in the fuel portion of the reactor.");
  params.addRequiredParam<FunctionName>(
      "gamma", "The ratio of power density generated in the moderator vs. the fuel.");
  return params;
}

GammaHeatSource::GammaHeatSource(const InputParameters & parameters)
  : Kernel(parameters),
    _average_fission_heat(getPostprocessorValue("average_fission_heat")),
    _gamma(getFunction("gamma"))
{
}

Real
GammaHeatSource::computeQpResidual()
{
  return -_test[_i][_qp] * _average_fission_heat * _gamma.value(_t, _q_point[_qp]);
}

Real
GammaHeatSource::computeQpJacobian()
{
  return 0;
}
