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

/**
 * The original intention of this aux kernel is to compute a delta T about some
 * nominal temperature used in the Boussinesq approximation for buoyancy in
 * incompressible Navier Stokes. The nominal temperature may need to change over
 * time in a simulation so that the relative size of the residual contributed by
 * the INSBoussinesqBodyForce stays around the same size as the other kernels.
 */
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
