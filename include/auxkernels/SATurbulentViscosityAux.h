#pragma once

#include "AuxKernel.h"

/*
 * Computes the turbulent dynamic viscosity for the Spalart-Allmaras turbulence model.
 */
class SATurbulentViscosityAux : public AuxKernel
{
public:
  static InputParameters validParams();

  SATurbulentViscosityAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const VariableValue & _mu_tilde;
  const ADMaterialProperty<Real> & _mu;
};
