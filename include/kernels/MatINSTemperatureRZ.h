#ifndef MATINSTEMPERATURERZ_H
#define MATINSTEMPERATURERZ_H

#include "Kernel.h"

// Forward Declarations
class MatINSTemperatureRZ;

template<>
InputParameters validParams<MatINSTemperatureRZ>();

/**
 * This class computes the residual and Jacobian contributions for the
 * incompressible Navier-Stokes temperature (energy) equation.
 */
class MatINSTemperatureRZ : public Kernel
{
public:
  MatINSTemperatureRZ(const InputParameters & parameters);

  virtual ~MatINSTemperatureRZ(){}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  // Coupled variables
  const VariableValue & _ur;
  const VariableValue & _uz;

  // Variable numberings
  unsigned _ur_var_number;
  unsigned _uz_var_number;

  // Required parameters
  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _k;
  const MaterialProperty<Real> & _cp;
};


#endif // MATINSTEMPERATURERZ_H
