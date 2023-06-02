#pragma once

#include "ADKernel.h"

/**
 * All terms in the turbulent viscosity equation of the Spalart-Allmaras model 
 */
class SATurbulentViscosity : public ADKernel
{
public:
  static InputParameters validParams();

  SATurbulentViscosity(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  const ADVectorVariableValue & _velocity;
  const ADVectorVariableGradient & _grad_velocity;
  const ADMaterialProperty<Real> & _mu;
  const ADMaterialProperty<Real> & _rho;
  const VariableValue & _wall_dist;
  const Real _sigma;
  const Real _cb1;
  const Real _cb2;
  const Real _kappa;
  const Real _cw1;
  const Real _cw2;
  const Real _cw3;
  const Real _cv1;
  const Real _ct1;
  const Real _ct2;
  const Real _ct3;
  const Real _ct4;
};
