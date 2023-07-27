#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel for computing the wall shear stress along a given boundary.
 */
class WallShearStressAux : public AuxKernel
{
public:
  static InputParameters validParams();

  WallShearStressAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const ADVectorVariableGradient & _grad_velocity;
  const ADMaterialProperty<Real> & _mu;
  const MooseArray<Point> & _normals;
};
