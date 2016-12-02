#ifndef CAMMIFUEL_H_
#define CAMMIFUEL_H_

#include "GenericMoltresMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"

class CammiFuel;

template<>
InputParameters validParams<CammiFuel>();

class CammiFuel : public GenericMoltresMaterial
{
public:
  CammiFuel(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

  MaterialProperty<Real> & _rho;
  MaterialProperty<Real> & _k;
};

#endif //CAMMIFUEL_H
