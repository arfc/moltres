#ifndef SIDEWEIGHTEDINTEGRALPOSTPROCESSOR_H
#define SIDEWEIGHTEDINTEGRALPOSTPROCESSOR_H

#include "SideIntegralVariablePostprocessor.h"

// Forward Declarations
class SideWeightedIntegralPostprocessor;

template <>
InputParameters validParams<SideIntegralVariablePostprocessor>();

/**
 * This postprocessor computes the weighted integral of a
 * variable along a boundary, weighted by the user-selected coupled variable.
 */
class SideWeightedIntegralPostprocessor : public SideIntegralVariablePostprocessor
{
public:
  static InputParameters validParams();

  SideWeightedIntegralPostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;
  virtual Real computeIntegral() override;

  // Weight variable
  const VariableValue & _weight;
};

#endif // SIDEWEIGHTEDINTEGRALPOSTPROCESSOR_H
