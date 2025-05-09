#include "IntegralNewVariablePostprocessor.h"

registerMooseObject("MoltresApp", IntegralNewVariablePostprocessor);

InputParameters
IntegralNewVariablePostprocessor::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredCoupledVar("variable", "The name of the variable that this object operates on");
  return params;
}

IntegralNewVariablePostprocessor::IntegralNewVariablePostprocessor(
    const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    ScalarTransportBase(parameters),
    MooseVariableInterface<Real>(this, false, "variable",
                                 Moose::VarKindType::VAR_ANY,
                                 Moose::VarFieldType::VAR_FIELD_STANDARD),
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
