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
  virtual Real computeQpJacobian() override;

  const ArrayVariableValue & _drift_var;
  const ArrayVariableGradient & _drift_grad;
  const bool _adaptive;
  const bool _use_jacobian;
  Point _bot_left;
  Point _top_right;
};
