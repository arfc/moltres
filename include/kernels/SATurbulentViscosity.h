#pragma once

#include "ADKernel.h"

/**
 * Computes residual contributions of all terms in the turbulent viscosity
 * equation of the Spalart-Allmaras model.
 */
class SATurbulentViscosity : public ADKernel
{
public:
  static InputParameters validParams();

  SATurbulentViscosity(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  const ADMaterialProperty<Real> & _mu;
  const ADMaterialProperty<Real> & _convection_strong_residual;
  const ADMaterialProperty<Real> & _destruction_strong_residual;
  const ADMaterialProperty<Real> & _diffusion_strong_residual;
  const ADMaterialProperty<Real> & _source_strong_residual;
};
