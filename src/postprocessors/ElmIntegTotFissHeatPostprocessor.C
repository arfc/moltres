#include "ElmIntegTotFissHeatPostprocessor.h"

registerMooseObject("MoltresApp", ElmIntegTotFissHeatPostprocessor);

InputParameters
ElmIntegTotFissHeatPostprocessor::validParams()
{
  InputParameters params = ElmIntegTotFissPostprocessor::validParams();
  return params;
}

ElmIntegTotFissHeatPostprocessor::ElmIntegTotFissHeatPostprocessor(
    const InputParameters & parameters)
  : ElmIntegTotFissPostprocessor(parameters),
    _fisse(getMaterialProperty<std::vector<Real>>("fisse"))
{
}

Real
ElmIntegTotFissHeatPostprocessor::computeFluxMultiplier(int index)
{
  return _fisse[_qp][index] * _fissxs[_qp][index];
}
