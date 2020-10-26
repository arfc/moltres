#ifndef POSTPROCESSORCOUPLEDINFLOWBC_H
#define POSTPROCESSORCOUPLEDINFLOWBC_H

#include "IntegratedBC.h"

class PostprocessorCoupledInflowBC;

template <>
InputParameters validParams<PostprocessorCoupledInflowBC>();

/**
 * This class computes the residual and Jacobian contributions for the
 * inflow boundary term in the advection-diffusion equation coupled to
 * Navier-Stokes velocity variables.
 */
class PostprocessorCoupledInflowBC : public IntegratedBC
{
public:
  PostprocessorCoupledInflowBC(const InputParameters & parameters);

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

#endif // POSTPROCESSORCOUPLEDINFLOWBC_H
