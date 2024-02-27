#pragma once

#include "AuxKernel.h"

/*
 * Computes the minimum wall distance of the element from the wall boundaries.
 */
class WallDistanceAux : public AuxKernel
{
public:
  static InputParameters validParams();

  WallDistanceAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  std::vector<BoundaryName> _wall_boundary_names;
};
