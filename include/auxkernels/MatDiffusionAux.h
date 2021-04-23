#pragma once

#include "AuxKernel.h"
#include "MaterialProperty.h"

/**
 * Computes the Euclidean norm of the flux of some diffusing variable at
 * any point in space. In other words, this computes:
 * \f[
 *   | D \nabla f |
 * \f]
 * Where D is the diffusion coefficient and f is some scalar variable in the simulation.
 */
class MatDiffusionAux : public AuxKernel
{
public:
  MatDiffusionAux(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue() override;

  std::string _prop_name;
  const VariableGradient & _grad_diffuse_var;
  const MaterialProperty<Real> * _diff;
};
