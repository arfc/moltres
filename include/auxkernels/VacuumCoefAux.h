#pragma once

#include "AuxKernel.h"

class VacuumCoefAux : public AuxKernel
{
public:
  static InputParameters validParams();

  VacuumCoefAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  /// Angular group flux variables
  const ArrayVariableValue & _psi;

  /// Boundary normals
  const MooseArray<Point> & _normals;

  /// Level-symmetric quadrature points
  RealEigenMatrix _ordinates;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;
};
