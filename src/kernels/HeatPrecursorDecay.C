#include "HeatPrecursorDecay.h"

registerMooseObject("MoltresApp", HeatPrecursorDecay);

template <>
InputParameters
validParams<HeatPrecursorDecay>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<unsigned int>("decay_heat_group_number",
                                        "What decay heat group this kernel is acting on.");
  params.addRequiredParam<RealVectorValue>("decay_heat_constants", "Decay Heat Constants");
  return params;
}

HeatPrecursorDecay::HeatPrecursorDecay(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _heat_group(getParam<unsigned int>("decay_heat_group_number") - 1),
    _decay_heat_const(getParam<RealVectorValue>("decay_heat_constants"))
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
  return _test[_i][_qp] * _decay_heat_const[_heat_group] * computeConcentrationDerivative(_u, phi, _j, _qp);
}

Real
HeatPrecursorDecay::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0;
}