#ifndef COUPLEDSCALARADVECTION_H
#define COUPLEDSCALARADVECTION_H

#include "Kernel.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class CoupledScalarAdvection;

template<>
InputParameters validParams<CoupledScalarAdvection>();

/**
 * This class computes the residual and Jacobian contributions for the
 * incompressible Navier-Stokes temperature (energy) equation.
 */
class CoupledScalarAdvection : public ScalarTransportBase<Kernel>
{
public:
  CoupledScalarAdvection(const InputParameters & parameters);

  virtual ~CoupledScalarAdvection(){}

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


#endif // COUPLEDSCALARADVECTION_H
