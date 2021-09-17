#include "HeatPrecursorDecay.h"

registerMooseObject("MoltresApp", HeatPrecursorDecay);

InputParameters
HeatPrecursorDecay::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredParam<unsigned int>("decay_heat_group_number",
                                        "The decay heat group this kernel acts on.");
  params.addRequiredParam<std::vector<Real>>("decay_heat_constants", "Decay Heat Constants");
  return params;
}

HeatPrecursorDecay::HeatPrecursorDecay(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _heat_group(getParam<unsigned int>("decay_heat_group_number") - 1),
    _decay_heat_const(getParam<std::vector<Real>>("decay_heat_constants"))
{
}

Real
HeatPrecursorDecay::computeQpResidual()
{
  return _test[_i][_qp] * _decay_heat_const[_heat_group] * computeConcentration(_u, _qp);
}

Real
HeatPrecursorDecay::computeQpJacobian()
{
  return _test[_i][_qp] * _decay_heat_const[_heat_group] * computeConcentrationDerivative(_u, _phi, _j, _qp);
}
