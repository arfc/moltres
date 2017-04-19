
#include "ElementL2Diff.h"

template <>
InputParameters
validParams<ElementL2Diff>()
{
  InputParameters params = validParams<ElementIntegralVariablePostprocessor>();
  return params;
}

ElementL2Diff::ElementL2Diff(const InputParameters & parameters)
  : ElementIntegralVariablePostprocessor(parameters), _u_old(valueOld())
{
}

Real
ElementL2Diff::getValue()
{
  return std::sqrt(ElementIntegralVariablePostprocessor::getValue());
}

Real
ElementL2Diff::computeQpIntegral()
{
  Real diff = _u[_qp] - _u_old[_qp];
  return diff * diff;
}
