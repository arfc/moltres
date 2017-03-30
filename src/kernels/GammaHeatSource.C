#include "GammaHeatSource.h"

template<>
InputParameters validParams<GammaHeatSource>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<PostprocessorName>("average_fission_heat", "The average fission heat being generated in the fuel portion of the reactor.");
  params.addRequiredParam<Real>("gamma", "The ratio of power density generated in the moderator vs. the fuel.");
  return params;
}

GammaHeatSource::GammaHeatSource(const InputParameters & parameters) :
    Kernel(parameters),
    _average_fission_heat(getPostprocessorValue("average_fission_heat")),
    _gamma(getParam<Real>("gamma"))
{
}

Real
GammaHeatSource::computeQpResidual()
{
  return -_test[_i][_qp] * _average_fission_heat * _gamma;
}

Real
GammaHeatSource::computeQpJacobian()
{
  return 0;
}
