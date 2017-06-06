#ifndef NUSSELT_H_
#define NUSSELT_H_

#include "Material.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"

class Nusselt;

template <>
InputParameters validParams<Nusselt>();

class Nusselt : public Material
{
public:
  Nusselt(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

  MaterialProperty<Real> & _k;
  Real _l_value;
  MaterialProperty<Real> & _nu;
  MaterialProperty<Real> & _h;
};

#endif // NUSSELT_H
