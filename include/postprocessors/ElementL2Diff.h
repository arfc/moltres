#pragma once

#include "ElementIntegralVariablePostprocessor.h"

class ElementL2Diff : public ElementIntegralVariablePostprocessor
{
public:
  ElementL2Diff(const InputParameters & parameters);

  static InputParameters validParams();

  using Postprocessor::getValue;
  virtual Real getValue() const override;

protected:
  virtual Real computeQpIntegral() override;

  const VariableValue & _u_old;
};
