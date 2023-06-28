#pragma once

#include "ADKernelStabilized.h"

/**
 * This class copmutes the momentum equation residual and Jacobian
 * contributions for CD stabilization terms of the incompressible Navier-Stokes momentum
 * equation.
 */
class INSADMomentumCD : public ADVectorKernelStabilized
{
public:
  static InputParameters validParams();

  INSADMomentumCD(const InputParameters & parameters);

  virtual void computeResidual() override;

protected:
  void computeResidualsForJacobian() override;
  virtual ADRealVectorValue precomputeQpStrongResidual() override;
  virtual ADRealVectorValue computeQpStabilization() override;

  const ADMaterialProperty<RealVectorValue> & _tau_cd;
  const ADVectorVariableValue & _velocity;
  const ADVectorVariableGradient & _grad_velocity;
  const ADMaterialProperty<RealVectorValue> & _momentum_strong_residual;
};
