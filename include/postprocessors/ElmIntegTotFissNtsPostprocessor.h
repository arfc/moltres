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

  int _num_groups;
  const MaterialProperty<std::vector<Real>> & _nsf;
  std::vector<MooseVariableFEBase *> _vars;
  std::vector<const VariableValue *> _group_fluxes;
};
