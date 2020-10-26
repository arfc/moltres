#include "SideCoupledIntegralVariablePostprocessor.h"

registerMooseObject("MoltresApp", SideCoupledIntegralVariablePostprocessor);

template <>
InputParameters
validParams<SideCoupledIntegralVariablePostprocessor>()
{
  InputParameters params = validParams<SideIntegralVariablePostprocessor>();
  params.addClassDescription("Postprocessor for calculating the weighted integral sum/average of a "
                             "variable along a boundary");
  params.addRequiredCoupledVar("weight",
                               "The variable to use as the weight function for calculating the weighted sum");
  params.addRequiredParam<PostprocessorName>("divisor",
                                             "Divisor value. For weighted sum, divisor = 1. "
                                             "For weighted average, provide the postprocessor name for "
                                             "the integral of the weight variable along the boundary.");
  return params;
}

SideCoupledIntegralVariablePostprocessor::SideCoupledIntegralVariablePostprocessor(
    const InputParameters & parameters)
  : SideIntegralVariablePostprocessor(parameters),
    _weight(coupledValue("weight")),
    _divisor(getPostprocessorValue("divisor"))
{
  addMooseVariableDependency(mooseVariable());
}

Real
SideCoupledIntegralVariablePostprocessor::computeQpIntegral()
{
  return _u[_qp] * _weight[_qp] / _divisor;
}

Real
SideCoupledIntegralVariablePostprocessor::computeIntegral()
{
  Real sum = 0;
  for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    sum += _JxW[_qp] * computeQpIntegral();
  return sum;
}
