#pragma once

#include "ElementIntegralVariablePostprocessor.h"

class ElementL2Diff : public ElementIntegralVariablePostprocessor
{
public:
  ElementL2Diff(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  /**
   * Get the L2 Error.
   */
  virtual Real getValue() const override;

  virtual Real computeQpIntegral() override;

  const VariableValue & _u_old;
};
