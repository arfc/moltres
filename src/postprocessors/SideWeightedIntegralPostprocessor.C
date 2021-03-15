#include "SideWeightedIntegralPostprocessor.h"

registerMooseObject("MoltresApp", SideWeightedIntegralPostprocessor);

template <>
InputParameters
validParams<SideWeightedIntegralPostprocessor>()
{
  InputParameters params = validParams<SideIntegralVariablePostprocessor>();
  params.addClassDescription("Postprocessor for calculating the weighted integral sum/average of a "
                             "variable along a boundary");
  params.addCoupledVar("weight",
                       1,
                       "The weight variable in the weighted integral, e.g. the velocity "
                       "variable for calculating flow-averaged temperature outflow");
  return params;
}

SideWeightedIntegralPostprocessor::SideWeightedIntegralPostprocessor(
    const InputParameters & parameters)
  : SideIntegralVariablePostprocessor(parameters),
    _weight(coupledValue("weight"))
{
  addMooseVariableDependency(mooseVariable());
}

Real
SideWeightedIntegralPostprocessor::computeQpIntegral()
{
  return _u[_qp] * _weight[_qp];
}
