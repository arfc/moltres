#pragma once

#include "DifferencePostprocessor.h"

/**
 * Computes the quotient between two postprocessors
 *
 * result = value1 / value2
 */
class DivisionPostprocessor : public DifferencePostprocessor
{
public:
  DivisionPostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

  using Postprocessor::getValue;
  virtual PostprocessorValue getValue() const override;

protected:
};
