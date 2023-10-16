#pragma once

#include "DGKernel.h"

/**
 * Computes residual contributions of the turbulent diffusion term in the delayed neutron
 * precursor equation using the discontinuous Galerkin method
 */
class DGTurbulentDiffusion : public DGKernel
{
public:
  static InputParameters validParams();

  DGTurbulentDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual(Moose::DGResidualType type) override;
  virtual Real computeQpJacobian(Moose::DGJacobianType type) override;

  Real _epsilon;
  Real _sigma;
  const ADMaterialProperty<Real> & _mu;
  const ADMaterialProperty<Real> & _mu_nb;
  const ADVariableValue & _mu_tilde;
  const ADVariableValue & _mu_tilde_nb;
  Real _sc;
};
