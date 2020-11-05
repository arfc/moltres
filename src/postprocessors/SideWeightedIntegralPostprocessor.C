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

Real
SideWeightedIntegralPostprocessor::computeIntegral()
{
  Real sum = 0;
  for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    sum += _JxW[_qp] * computeQpIntegral();
  return sum;
}
