#pragma once

#include "Kernel.h"

/**
 * This class computes momentum equation residual and Jacobian
 * contributions for the incompressible Navier-Stokes momentum
 * equation with a standard k-epsilon turbulence model.
 */
class INSMomentumKEpsilon : public Kernel
{
public:
  INSMomentumKEpsilon(const InputParameters & parameters);

  static InputParameters validParams();

  virtual ~INSMomentumKEpsilon() {}

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned jvar) override;

  // Coupled variables
  const VariableValue & _u_vel;
  const VariableValue & _v_vel;
  const VariableValue & _w_vel;
  const VariableValue & _p;
  const VariableValue & _k;

  // Gradients
  const VariableGradient & _grad_u_vel;
  const VariableGradient & _grad_v_vel;
  const VariableGradient & _grad_w_vel;
  const VariableGradient & _grad_p;
  const VariableGradient & _grad_k;

  // Variable numberings
  unsigned int _u_vel_var_number;
  unsigned int _v_vel_var_number;
  unsigned int _w_vel_var_number;
  unsigned int _p_var_number;
  unsigned int _k_var_number;

  // Material properties
  // MaterialProperty<Real> & _dynamic_viscosity;
  Real _mu;
  Real _rho;
  RealVectorValue _gravity;

  // Parameters
  unsigned _component;
  bool _integrate_p_by_parts;
};
