#pragma once

#include "ADDiffusion.h"

/**
 * Computes residual contributions of the turbulent diffusion term in the INSAD energy equation.
 */
class INSADHeatTurbulentDiffusion : public ADDiffusion
{
public:
  static InputParameters validParams();

  INSADHeatTurbulentDiffusion(const InputParameters & parameters);

protected:
  ADRealVectorValue precomputeQpResidual() override;

  const ADMaterialProperty<Real> & _cp;
  const ADMaterialProperty<Real> & _mu;
  const ADVariableValue & _mu_tilde;
  const Real _pr;
};
