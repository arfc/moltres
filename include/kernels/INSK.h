#ifndef INSK_H
#define INSK_H

#include "Kernel.h"

// Forward Declarations
class INSK;

template <>
InputParameters validParams<INSK>();

/**
 * This class computes turbulent energy residual and Jacobian
 * contributions for the incompressible Navier-Stokes momentum
 * equation with a standard k-epsilon turbulence model.
 */
class INSK : public Kernel
{
public:
  INSK(const InputParameters & parameters);

  virtual ~INSK() {}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  // Coupled variables
  const VariableValue & _u_vel;
  const VariableValue & _v_vel;
  const VariableValue & _w_vel;
  const VariableValue & _epsilon;

  // Gradients
  const VariableGradient & _grad_u_vel;
  const VariableGradient & _grad_v_vel;
  const VariableGradient & _grad_w_vel;
  const VariableGradient & _grad_epsilon;

  // Variable numberings
  unsigned int _u_vel_var_number;
  unsigned int _v_vel_var_number;
  unsigned int _w_vel_var_number;
  unsigned int _epsilon_var_number;

  // Material properties
  // MaterialProperty<Real> & _dynamic_viscosity;
  Real _mu;
  Real _rho;
  Real _Cmu;
  Real _sigk;
  Real _sigeps;
  Real _C1eps;
  Real _C2eps;
};

#endif // INSK_H
