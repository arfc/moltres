#pragma once

#include "IntegratedBC.h"
#include "ScalarTransportBase.h"

/**
 * This class computes the residual and Jacobian contributions for the
 * incompressible Navier-Stokes temperature (energy) equation.
 */
class ScalarAdvectionArtDiffNoBCBC : public IntegratedBC, public ScalarTransportBase
{
public:
  ScalarAdvectionArtDiffNoBCBC(const InputParameters & parameters);

  static InputParameters validParams();

  virtual ~ScalarAdvectionArtDiffNoBCBC() {}

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned jvar) override;

  Real _scale;
  // Coupled variables
  const VariableValue & _u_vel;
  const VariableValue & _v_vel;
  const VariableValue & _w_vel;

  // Variable numberings
  unsigned _u_vel_var_number;
  unsigned _v_vel_var_number;
  unsigned _w_vel_var_number;

  VariableValue _u_def;
  VariableValue _v_def;
  VariableValue _w_def;
  Real _conc_scaling;
  Real _tau;
};
