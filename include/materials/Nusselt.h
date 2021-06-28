#pragma once

#include "Material.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"

class Nusselt : public Material
{
public:
  Nusselt(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void computeQpProperties() override;

  const MaterialProperty<Real> & _k;
  Real _l_value;
  MaterialProperty<Real> & _nu;
  MaterialProperty<Real> & _h;
};
