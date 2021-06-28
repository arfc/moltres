#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class DecayHeatSource : public Kernel, public ScalarTransportBase
{
public:
  DecayHeatSource(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  unsigned int _num_heat_groups;
  std::vector<Real> _decay_heat_const;
  std::vector<const VariableValue *> _heat_concs;
  std::vector<unsigned int> _heat_ids;
};
