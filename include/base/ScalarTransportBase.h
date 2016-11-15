#ifndef SCALARTRANSPORTBASE_H
#define SCALARTRANSPORTBASE_H

// #include "MooseObject.h"
// #include "InputParameters.h"

class InputParameters
class ScalarTransportBase
class VariableValue
class VariableGradient
class VariablePhiValue
class VarablePhiGradient

<template>
InputParameters validParams<ScalarTransportBase>();

class ScalarTransportBase
{
public:
  ScalarTransportBase(const InputParameters & parameters);

  Real computeConcentration(const VariableValue & u, unsigned int qp);
  RealVectorValue computeConcentrationGradient(const VariableGradient & grad_u, unsigned int qp);
  Real computeConcentrationDerivative(const VariableValue & u, const VariablePhiValue & phi,
                                      unsigned int j, unsigned int qp);
  RealVectorValue computeConcentrationGradientDerivative(const VariableValue & u, const VariableGradient & grad_u,
                                                         const VariablePhiValue & phi, const VariablePhiGradient & grad_phi,
                                                         unsigned int j, unsigned int qp);
  Real computeConcentrationDot(const VariableValue & u, const VariableValue u_dot, unsigned int qp);
  Real computeConcentrationDotDerivative(const VariableValue & u, const VariableValue & u_dot,
                                         const VariableValue & du_dot_du, const VariablePhiValue & phi,
                                         unsigned int j, unsigned int qp);

private:
  const bool _use_exp_form;
};

#endif //SCALARTRANSPORTBASE_H
