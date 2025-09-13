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

  /// Neutron drift variable
  const ArrayVariableValue & _drift_var;

  /// Neutron drift variable gradient
  const ArrayVariableGradient & _drift_grad;

  /// Whether to apply hybrid method adaptive boundary coupling
  const bool _adaptive;

  /// Whether to include neutron drift Jacobian contributions
  const bool _use_jacobian;

  /// Bounding box bottom left coordinates
  Point _bot_left;

  /// Bounding box top right coordinates
  Point _top_right;
};
