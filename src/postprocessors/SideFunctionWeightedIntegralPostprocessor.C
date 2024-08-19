#include "SideFunctionWeightedIntegralPostprocessor.h"
#include "Function.h"

registerMooseObject("MoltresApp", SideFunctionWeightedIntegralPostprocessor);

InputParameters
SideFunctionWeightedIntegralPostprocessor::validParams()
{
  InputParameters params = SideIntegralVariablePostprocessor::validParams();
  params.addClassDescription("Postprocessor for calculating the weighted integral sum/average of a "
                             "variable along a boundary");
  params.addParam<FunctionName>("weight_func",
                                1,
                                "The weight function in the weighted integral, e.g. the velocity "
                                "variable for calculating flow-averaged temperature outflow");
  return params;
}

SideFunctionWeightedIntegralPostprocessor::SideFunctionWeightedIntegralPostprocessor(
    const InputParameters & parameters)
  : SideIntegralVariablePostprocessor(parameters),
    _weight_func(getFunction("weight_func"))
{
}

Real
SideFunctionWeightedIntegralPostprocessor::computeQpIntegral()
{
  return _u[_qp] * _weight_func.value(_t, _q_point[_qp]);
}
