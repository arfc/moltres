#ifndef SIDECOUPLEDINTEGRALVARIABLEPOSTPROCESSOR_H
#define SIDECOUPLEDINTEGRALVARIABLEPOSTPROCESSOR_H

#include "SideIntegralVariablePostprocessor.h"

// Forward Declarations
class SideCoupledIntegralVariablePostprocessor;

template <>
InputParameters validParams<SideIntegralVariablePostprocessor>();

/**
 * This postprocessor computes the weighted integral sum/average of a
 * variable along a boundary, weighted by the user-provided coupled variable.
 * If the user provides divisor = 1,
 * this postprocessor returns the weighted integral sum. For the weighted
 * integral average, the user must provide the unweighted integral sum of the
 * weight variable of this postprocessor.
 */
class SideCoupledIntegralVariablePostprocessor : public SideIntegralVariablePostprocessor
{
public:
  static InputParameters validParams();

  SideCoupledIntegralVariablePostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;
  virtual Real computeIntegral() override;

  const VariableValue & _weight;
  const PostprocessorValue & _divisor;
};

#endif // SIDECOUPLEDINTEGRALVARIABLEPOSTPROCESSOR_H
