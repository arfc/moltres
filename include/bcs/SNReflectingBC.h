#pragma once

#include "ArrayIntegratedBC.h"

class SNReflectingBC : public ArrayIntegratedBC
{
public:
  static InputParameters validParams();

  SNReflectingBC(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;
  virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;

  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
  std::vector<int> _x_reflection;
  std::vector<int> _y_reflection;
  std::vector<int> _z_reflection;
};
