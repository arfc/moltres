#include "DivisionPostprocessor.h"

registerMooseObject("MoltresApp", DivisionPostprocessor);

InputParameters
DivisionPostprocessor::validParams()
{
  InputParameters params = DifferencePostprocessor::validParams();
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
