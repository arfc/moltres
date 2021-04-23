#include "IntegralOldVariablePostprocessor.h"

registerMooseObject("MoltresApp", IntegralOldVariablePostprocessor);

InputParameters
IntegralOldVariablePostprocessor::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredCoupledVar("variable", "The name of the variable that this object operates on");
  return params;
}

IntegralOldVariablePostprocessor::IntegralOldVariablePostprocessor(
    const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    ScalarTransportBase(parameters),
    MooseVariableInterface<Real>(this, false, "variable",
                                 Moose::VarKindType::VAR_ANY,
                                 Moose::VarFieldType::VAR_FIELD_STANDARD),
    _u_old(coupledValueOld("variable")),
    _grad_u_old(coupledGradientOld("variable"))
{
  addMooseVariableDependency(mooseVariable());
}

Real
IntegralOldVariablePostprocessor::computeQpIntegral()
{
  return computeConcentration(_u_old, _qp);
}
