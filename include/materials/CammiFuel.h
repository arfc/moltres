#pragma once

#include "GenericMoltresMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"

class CammiFuel : public GenericMoltresMaterial
{
public:
  CammiFuel(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void computeQpProperties() override;

  MaterialProperty<Real> & _rho;
  MaterialProperty<Real> & _k;
};
