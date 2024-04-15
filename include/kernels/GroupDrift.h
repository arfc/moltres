#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class GroupDrift : public Kernel, public ScalarTransportBase
{
public:
  GroupDrift(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;

  const ArrayVariableValue & _drift_var;
};
