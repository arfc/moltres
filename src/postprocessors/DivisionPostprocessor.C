#include "DivisionPostprocessor.h"

template <>
InputParameters
validParams<DivisionPostprocessor>()
{
  InputParameters params = validParams<DifferencePostprocessor>();
  return params;
}

DivisionPostprocessor::DivisionPostprocessor(const InputParameters & parameters)
  : DifferencePostprocessor(parameters)
{
}

PostprocessorValue
DivisionPostprocessor::getValue()
{
  return _value1 / _value2;
}
