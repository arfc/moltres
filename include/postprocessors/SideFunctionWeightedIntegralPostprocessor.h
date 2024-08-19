#pragma once

#include "SideIntegralVariablePostprocessor.h"

/**
 * This postprocessor computes the weighted integral of a
 * variable along a boundary, weighted by the user-provided function.
 */
class SideFunctionWeightedIntegralPostprocessor : public SideIntegralVariablePostprocessor
{
public:
  SideFunctionWeightedIntegralPostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpIntegral() override;

  // Weight function
  const Function & _weight_func;
};
