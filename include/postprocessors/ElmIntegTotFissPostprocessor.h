#ifndef ELMINTEGTOTFISSPOSTPROCESSOR_H
#define ELMINTEGTOTFISSPOSTPROCESSOR_H

#include "ElementIntegralPostprocessor.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class ElmIntegTotFissPostprocessor;

template <>
InputParameters validParams<ElmIntegTotFissPostprocessor>();

class ElmIntegTotFissPostprocessor : public ElementIntegralPostprocessor, public ScalarTransportBase
{

public:
  ElmIntegTotFissPostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;
  virtual Real computeFluxMultiplier(int index);

  int _num_groups;
  const MaterialProperty<std::vector<Real>> & _fissxs;
  std::vector<MooseVariableFEBase *> _vars;
  Real _nt_scale;
  std::vector<const VariableValue *> _group_fluxes;
};

#endif
