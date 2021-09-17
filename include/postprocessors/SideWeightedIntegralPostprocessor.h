#pragma once

#include "SideIntegralVariablePostprocessor.h"

/**
 * This postprocessor computes the weighted integral of a
 * variable along a boundary, weighted by the user-selected coupled variable.
 */
class SideWeightedIntegralPostprocessor : public SideIntegralVariablePostprocessor
{
public:
  SideWeightedIntegralPostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpIntegral() override;

  // Weight variable
  const VariableValue & _weight;
};
