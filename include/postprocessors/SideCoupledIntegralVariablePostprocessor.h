#ifndef SIDECOUPLEDINTEGRALVARIABLEPOSTPROCESSOR_H
#define SIDECOUPLEDINTEGRALVARIABLEPOSTPROCESSOR_H

#include "SideIntegralVariablePostprocessor.h"

// Forward Declarations
class SideCoupledIntegralVariablePostprocessor;

template <>
InputParameters validParams<SideIntegralVariablePostprocessor>();

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
