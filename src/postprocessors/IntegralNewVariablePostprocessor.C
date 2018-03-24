#include "IntegralNewVariablePostprocessor.h"

registerMooseObject("MoltresApp", IntegralNewVariablePostprocessor);

template <>
InputParameters
validParams<IntegralNewVariablePostprocessor>()
{
  InputParameters params = validParams<ElementIntegralPostprocessor>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredCoupledVar("variable", "The name of the variable that this object operates on");
  return params;
}

IntegralNewVariablePostprocessor::IntegralNewVariablePostprocessor(
    const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    ScalarTransportBase(parameters),
    MooseVariableInterface<Real>(this, false),
    _u(coupledValue("variable")),
    _grad_u(coupledGradient("variable")),
    _u_dot(coupledDot("variable"))
{
  addMooseVariableDependency(mooseVariable());
}

Real
IntegralNewVariablePostprocessor::computeQpIntegral()
{
  return computeConcentration(_u, _qp);
}
