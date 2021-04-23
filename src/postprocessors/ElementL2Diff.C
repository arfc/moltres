#include "ElementL2Diff.h"

registerMooseObject("MoltresApp", ElementL2Diff);

InputParameters
ElementL2Diff::validParams()
{
  InputParameters params = ElementIntegralVariablePostprocessor::validParams();
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
