#include "IntegralOldVariablePostprocessor.h"

template<>
InputParameters validParams<IntegralOldVariablePostprocessor>()
{
  InputParameters params = validParams<ElementIntegralPostprocessor>();
  params.addRequiredCoupledVar("variable", "The name of the variable that this object operates on");
  return params;
}

IntegralOldVariablePostprocessor::IntegralOldVariablePostprocessor(const InputParameters & parameters) :
    ElementIntegralPostprocessor(parameters),
    MooseVariableInterface(this, false),
    _u_old(coupledValueOld("variable")),
    _grad_u_old(coupledGradientOld("variable"))
{
  addMooseVariableDependency(mooseVariable());
}

Real
IntegralOldVariablePostprocessor::computeQpIntegral()
{
  return _u_old[_qp];
}
