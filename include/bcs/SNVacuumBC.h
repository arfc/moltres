#pragma once

#include "ArrayIntegratedBC.h"

class SNVacuumBC : public ArrayIntegratedBC
{
public:
  static InputParameters validParams();

  SNVacuumBC(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;

  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
