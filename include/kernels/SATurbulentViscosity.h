#pragma once

#include "INSFEFluidKernelBase.h"

/**
 * All terms in the turbulent viscosity equation of the Spalart-Allmaras model 
 */
class SATurbulentViscosity : public INSFEFluidKernelBase
{
public:
  static InputParameters validParams();

  SATurbulentViscosity(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const std::vector<BoundaryName> _wall_boundary_names;
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
