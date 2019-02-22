#ifndef SCALARTRANSPORTBASE_H
#define SCALARTRANSPORTBASE_H

#include "InputParameters.h"
#include "MooseVariableBase.h"

class ScalarTransportBase;

template <>
InputParameters validParams<ScalarTransportBase>();

/**
 * This class is useful for calculating the concentration, independent
 * variable derivatives, and Jacobian contributions for a scalar variable that may
 * be in either a linear or logarithmic form. E.g. in the governing equation,
 * the actual concentration may be either \f$c = u\f$ or \f$c = e^u\f$ respectively.
 */
class ScalarTransportBase
{
public:
  ScalarTransportBase(const InputParameters & parameters);

  /// Computes \f$c\f$
  virtual Real computeConcentration(const VariableValue & u, unsigned int qp);

  /// Computes \f$\nabla c\f$
  virtual RealVectorValue computeConcentrationGradient(const VariableValue & u,
                                                       const VariableGradient & grad_u,
                                                       unsigned int qp);

  /// Computes \f$\frac{\partial c}{\partial u_j}\f$
  virtual Real computeConcentrationDerivative(const VariableValue & u,
                                              const VariablePhiValue & phi,
                                              unsigned int j,
                                              unsigned int qp);

  /// Computes \f$\nabla \frac{\partial c}{\partial u_j}\f$
  virtual RealVectorValue
  computeConcentrationGradientDerivative(const VariableValue & u,
                                         const VariableGradient & grad_u,
                                         const VariablePhiValue & phi,
                                         const VariablePhiGradient & grad_phi,
                                         unsigned int j,
                                         unsigned int qp);

  /// Computes \f$\frac{\partial c}{\partial t}\f$
  virtual Real
  computeConcentrationDot(const VariableValue & u, const VariableValue & u_dot, unsigned int qp);

  /// Computes \f$\frac{\partial}{\partial t} \frac{\partial c}{\partial u_j}\f$
  virtual Real computeConcentrationDotDerivative(const VariableValue & u,
                                                 const VariableValue & u_dot,
                                                 const VariableValue & du_dot_du,
                                                 const VariablePhiValue & phi,
                                                 unsigned int j,
                                                 unsigned int qp);

private:
  /// Boolean flag that determines whether to use the exponential/logarithmic formulation
  const bool _use_exp_form;
};

#endif // SCALARTRANSPORTBASE_H
