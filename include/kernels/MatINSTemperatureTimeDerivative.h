#ifndef MATINSTEMPERATURETIMEDERIVATIVE_H
#define MATINSTEMPERATURETIMEDERIVATIVE_H

#include "TimeDerivative.h"

// Forward Declarations
class MatINSTemperatureTimeDerivative;

template<>
InputParameters validParams<MatINSTemperatureTimeDerivative>();

/**
 * This class computes the time derivative for the incompressible
 * Navier-Stokes momentum equation.
 */
class MatINSTemperatureTimeDerivative : public TimeDerivative
{
public:
  MatINSTemperatureTimeDerivative(const InputParameters & parameters);

  virtual ~MatINSTemperatureTimeDerivative(){}

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned jvar);

  // Parameters
  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _cp;
};


#endif // MATINSTEMPERATURETIMEDERIVATIVE_H
