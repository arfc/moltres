#pragma once

#include "AuxKernel.h"

class SNScalarFluxAux : public AuxKernel
{
public:
  static InputParameters validParams();

  SNScalarFluxAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  const ArrayVariableValue & _psi;
  RealEigenVector _weights;
};
