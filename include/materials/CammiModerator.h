#pragma once

#include "GenericMoltresMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"

class CammiModerator : public GenericMoltresMaterial
{
public:
  CammiModerator(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void computeQpProperties() override;

  MaterialProperty<Real> & _k;
};
