#include "SideCoupledIntegralVariablePostprocessor.h"

registerMooseObject("MoltresApp", SideCoupledIntegralVariablePostprocessor);

template <>
InputParameters
validParams<SideCoupledIntegralVariablePostprocessor>()
{
  InputParameters params = validParams<SideIntegralVariablePostprocessor>();
  params.addRequiredCoupledVar("weight",
                               "The name of the variable that this postprocessor is integrated with");
  params.addRequiredParam<PostprocessorName>("divisor",
                                             "The postprocessor value of the divisor to normalize by");
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
