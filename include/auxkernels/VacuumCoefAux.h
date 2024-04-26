#pragma once

#include "AuxKernel.h"

class VacuumCoefAux : public AuxKernel
{
public:
  static InputParameters validParams();

  VacuumCoefAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  const unsigned int _N;
  const ArrayVariableValue & _psi;
  const MooseArray<Point> & _normals;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
