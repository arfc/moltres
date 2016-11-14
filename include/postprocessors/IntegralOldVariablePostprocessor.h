#ifndef INTEGRALOLDVARIABLEPOSTPROCESSOR_H
#define INTEGRALOLDVARIABLEPOSTPROCESSOR_H

#include "ElementIntegralPostprocessor.h"
#include "MooseVariableInterface.h"

//Forward Declarations
class IntegralOldVariablePostprocessor;

template<>
InputParameters validParams<IntegralOldVariablePostprocessor>();

/**
 * This postprocessor computes a volume integral of the specified variable from the
 * previous time step.
 *
 * Note that specializations of this integral are possible by deriving from this
 * class and overriding computeQpIntegral().
 */
class IntegralOldVariablePostprocessor :
  public ElementIntegralPostprocessor,
  public MooseVariableInterface
{
public:
  IntegralOldVariablePostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

  /// Holds the solution at current quadrature points
  const VariableValue & _u_old;
  /// Holds the solution gradient at the current quadrature points
  const VariableGradient & _grad_u_old;
};

#endif
