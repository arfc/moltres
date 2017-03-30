#include "ElmIntegTotFissHeatPostprocessor.h"

template<>
InputParameters validParams<ElmIntegTotFissHeatPostprocessor>()
{
  InputParameters params = validParams<ElmIntegTotFissPostprocessor>();
  return params;
}

ElmIntegTotFissHeatPostprocessor::ElmIntegTotFissHeatPostprocessor(const InputParameters & parameters) :
    ElmIntegTotFissPostprocessor(parameters),
    _fisse(getMaterialProperty<std::vector<Real> >("fisse"))
{
}

Real
ElmIntegTotFissHeatPostprocessor::computeFluxMultiplier(int index)
{
  return _fisse[_qp][index] * _fissxs[_qp][index];
}
