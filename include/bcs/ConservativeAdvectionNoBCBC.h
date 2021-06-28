#pragma once

#include "IntegratedBC.h"

class ConservativeAdvectionNoBCBC : public IntegratedBC
{
public:
  ConservativeAdvectionNoBCBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  RealVectorValue _velocity;
};
