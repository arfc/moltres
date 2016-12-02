#ifndef CAMMIMODERATOR_H_
#define CAMMIMODERATOR_H_

#include "GenericMoltresMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"

class CammiModerator;

template<>
InputParameters validParams<CammiModerator>();

class CammiModerator : public GenericMoltresMaterial
{
public:
  CammiModerator(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

  MaterialProperty<Real> & _k;
};

#endif //CAMMIMODERATOR_H
