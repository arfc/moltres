#ifndef SCALARTRANSPORTBASE_H
#define SCALARTRANSPORTBASE_H

#include "InputParameters.h"
#include "MooseVariableBase.h"

class ScalarTransportBase;

template<>
InputParameters validParams<ScalarTransportBase>();

class ScalarTransportBase
{
public:
  ScalarTransportBase(const InputParameters & parameters);

  virtual Real computeConcentration(const VariableValue & u, unsigned int qp);

  virtual RealVectorValue computeConcentrationGradient(const VariableValue & u, const VariableGradient & grad_u, unsigned int qp);

  virtual Real computeConcentrationDerivative(const VariableValue & u, const VariablePhiValue & phi,
                                      unsigned int j, unsigned int qp);

  virtual RealVectorValue computeConcentrationGradientDerivative(const VariableValue & u, const VariableGradient & grad_u,
                                                         const VariablePhiValue & phi, const VariablePhiGradient & grad_phi,
                                                         unsigned int j, unsigned int qp);

  virtual Real computeConcentrationDot(const VariableValue & u, const VariableValue u_dot, unsigned int qp);

  virtual Real computeConcentrationDotDerivative(const VariableValue & u, const VariableValue & u_dot,
                                         const VariableValue & du_dot_du, const VariablePhiValue & phi,
                                         unsigned int j, unsigned int qp);

private:
  const bool _use_exp_form;
};

#endif //SCALARTRANSPORTBASE_H
