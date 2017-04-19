#ifndef COUPLEDSCALARADVECTIONNOBCBC_H
#define COUPLEDSCALARADVECTIONNOBCBC_H

#include "IntegratedBC.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class CoupledScalarAdvectionNoBCBC;

template <>
InputParameters validParams<CoupledScalarAdvectionNoBCBC>();

/**
 * This class computes the residual and Jacobian contributions for the
 * incompressible Navier-Stokes temperature (energy) equation.
 */
class CoupledScalarAdvectionNoBCBC : public IntegratedBC, public ScalarTransportBase
{
public:
  CoupledScalarAdvectionNoBCBC(const InputParameters & parameters);

  virtual ~CoupledScalarAdvectionNoBCBC() {}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

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
};

#endif // COUPLEDSCALARADVECTIONNOBCBC_H
