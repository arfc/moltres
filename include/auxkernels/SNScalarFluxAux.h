#pragma once

#include "AuxKernel.h"

class SNScalarFluxAux : public AuxKernel
{
public:
  static InputParameters validParams();

  SNScalarFluxAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  const unsigned int _N;
  const ArrayVariableValue & _psi;
  RealEigenVector _weights;
};
