#pragma once

#include "ElementIntegralPostprocessor.h"
#include "MooseVariableInterface.h"
#include "ScalarTransportBase.h"

/**
 * This postprocessor computes a volume integral of the specified variable.
 *
 * Note that specializations of this integral are possible by deriving from this
 * class and overriding computeQpIntegral().
 */
class IntegralNewVariablePostprocessor : public ElementIntegralPostprocessor,
                                         public ScalarTransportBase,
                                         public MooseVariableInterface<Real>
{
public:
  IntegralNewVariablePostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpIntegral() override;

  /// Holds the solution at current quadrature points
  const VariableValue & _u;
  /// Holds the solution gradient at the current quadrature points
  const VariableGradient & _grad_u;
  /// Holds the solution derivative at the current quadrature points
  const VariableValue & _u_dot;
};
