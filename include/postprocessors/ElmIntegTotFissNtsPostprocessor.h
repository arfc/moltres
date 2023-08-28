#pragma once

#include "ElementIntegralPostprocessor.h"
#include "MooseVariableInterface.h"

class ElmIntegTotFissNtsPostprocessor : public ElementIntegralPostprocessor
/* public MooseVariableInterface */
{
public:
  ElmIntegTotFissNtsPostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpIntegral() override;

  // The number of neutron energy groups.
  int _num_groups;

  // The number of precursor groups.
  int _num_precursor_groups;

  // Whether to account for delayed neutrons
  bool _account_delayed;

  // nu Sigma_f material property
  const MaterialProperty<std::vector<Real>> & _nsf;

  // Decay constant material property
  const MaterialProperty<std::vector<Real>> & _decay_constant;

  std::vector<MooseVariableFEBase *> _vars;

  // Group flux variables
  std::vector<const VariableValue *> _group_fluxes;

  // Precursor concentration variables
  std::vector<const VariableValue *> _pre_concs;
};
