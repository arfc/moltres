#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel for computing the norm of turbulent stress
 */
class TurbulentStressAux : public AuxKernel
{
public:
  static InputParameters validParams();

  TurbulentStressAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const ADVariableValue & _mu_tilde;
  const ADMaterialProperty<Real> & _mu;
  const ADMaterialProperty<Real> & _rho;
  const ADMaterialProperty<Real> & _strain_mag;
};
