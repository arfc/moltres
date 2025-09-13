#pragma once

#include "IntegratedBC.h"
#include "ScalarTransportBase.h"

class VacuumCorrectionBC : public IntegratedBC, public ScalarTransportBase
{
public:
  VacuumCorrectionBC(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  const VariableValue & _coef_var;
};
