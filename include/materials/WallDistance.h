#pragma once

#include "Material.h"

/**
 * This object computes the minimum wall distance of each point from wall boundaries
 */
class WallDistance : public Material
{
public:
  static InputParameters validParams();

  WallDistance(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  std::vector<BoundaryName> _wall_boundary_names;
  MaterialProperty<Real> & _wall_dist;
  bool _compute;
};
