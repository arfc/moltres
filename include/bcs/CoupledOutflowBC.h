#ifndef COUPLEDOUTFLOWBC_H
#define COUPLEDOUTFLOWBC_H

#include "IntegratedBC.h"

// Forward Declarations
class CoupledOutflowBC;

template<>
InputParameters validParams<CoupledOutflowBC>();

/**
 * This class computes the residual and Jacobian contributions for the
 * DG outflow boundary term in the advection-diffusion equation
 * coupled to Navier-Stokes velocity variables.
 */
class CoupledOutflowBC : public IntegratedBC
{
public:
  CoupledOutflowBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

private:
  // Velocity variables
  const VariableValue & _vel_x;
  const VariableValue & _vel_y;
  const VariableValue & _vel_z;
};

#endif // COUPLEDOUTFLOWBC_H
