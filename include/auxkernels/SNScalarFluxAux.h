#pragma once

#include "AuxKernel.h"

class SNScalarFluxAux : public AuxKernel
{
public:
  static InputParameters validParams();

  SNScalarFluxAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  /// Angular group flux variables
  const ArrayVariableValue & _psi;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;
};
