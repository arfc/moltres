#include "ConstantDifferenceAux.h"

template <>
InputParameters
validParams<ConstantDifferenceAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addParam<Real>("constant", 0.0, "Scalar value subtracted from compareVar");
  params.addRequiredCoupledVar("compareVar", "Coupled variable");
  params.declareControllable("constant");
  return params;
}

ConstantDifferenceAux::ConstantDifferenceAux(const InputParameters & parameters)
  : AuxKernel(parameters),

    // We can couple in a value from one of our kernels with a call to coupledValueAux
    _variable(coupledValue("compareVar")),

    // Set our member scalar value from InputParameters (read from the input file)
    _constant(getParam<Real>("constant"))
{
}

Real
ConstantDifferenceAux::computeValue()
{
  return _variable[_qp] - _constant;
}
