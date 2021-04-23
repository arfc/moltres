#pragma once

#include "ElementIntegralPostprocessor.h"
#include "ScalarTransportBase.h"
#include "MooseVariableInterface.h"

/**
 * This postprocessor computes a volume integral of the specified variable from the
 * previous time step.
 *
 * Note that specializations of this integral are possible by deriving from this
 * class and overriding computeQpIntegral().
 */
class IntegralOldVariablePostprocessor : public ElementIntegralPostprocessor,
                                         public ScalarTransportBase,
                                         public MooseVariableInterface<Real>
{
public:
  IntegralOldVariablePostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpIntegral() override;

  /// Holds the solution at current quadrature points
  const VariableValue & _u_old;
  /// Holds the solution gradient at the current quadrature points
  const VariableGradient & _grad_u_old;
};
