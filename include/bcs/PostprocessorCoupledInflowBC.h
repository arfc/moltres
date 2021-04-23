#pragma once

#include "IntegratedBC.h"

/**
 * This class computes the residual and Jacobian contributions for the
 * inflow boundary term in the advection-diffusion equation coupled to
 * Navier-Stokes velocity variables.
 */
class PostprocessorCoupledInflowBC : public IntegratedBC
{
public:
  PostprocessorCoupledInflowBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  const PostprocessorValue & _pp_value;
  const Real & _scale;
  const Real & _offset;
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

private:
  // Velocity variables
  const VariableValue & _vel_x;
  const VariableValue & _vel_y;
  const VariableValue & _vel_z;
};
