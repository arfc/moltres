#ifndef SCALARADVECTIONARTDIFF_H
#define SCALARADVECTIONARTDIFF_H

#include "Kernel.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class ScalarAdvectionArtDiff;

template <>
InputParameters validParams<ScalarAdvectionArtDiff>();

/**
 * This class computes the residual and Jacobian contributions for the
 * incompressible Navier-Stokes temperature (energy) equation.
 */
class ScalarAdvectionArtDiff : public Kernel, public ScalarTransportBase
{
public:
  ScalarAdvectionArtDiff(const InputParameters & parameters);

  virtual ~ScalarAdvectionArtDiff() {}

protected:
  virtual Real tau();
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
  const MaterialProperty<Real> & _D;
  Real _conc_scaling;
  const Real & _current_elem_volume;
};

#endif // SCALARADVECTIONARTDIFF_H
