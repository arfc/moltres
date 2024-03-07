#pragma once

#include "ElementIntegralPostprocessor.h"
#include "ScalarTransportBase.h"

class ElmIntegTotFissPostprocessor : public ElementIntegralPostprocessor, public ScalarTransportBase
{
public:
  ElmIntegTotFissPostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpIntegral() override;
  virtual Real computeFluxMultiplier(int index);

  unsigned int _num_groups;
  const MaterialProperty<std::vector<Real>> & _fissxs;
  std::vector<MooseVariableFEBase *> _vars;
  Real _nt_scale;
  std::vector<const VariableValue *> _group_fluxes;
};
