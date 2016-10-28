#ifndef ELMINTEGTOTFISSPOSTPROCESSOR_H
#define ELMINTEGTOTFISSPOSTPROCESSOR_H

#include "ElementIntegralPostprocessor.h"
#include "MooseVariableInterface.h"

//Forward Declarations
class ElmIntegTotFissPostprocessor;

template<>
InputParameters validParams<ElmIntegTotFissPostprocessor>();

class ElmIntegTotFissPostprocessor :
  public ElementIntegralPostprocessor
  /* public MooseVariableInterface */
{
public:
  ElmIntegTotFissPostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

  int _num_groups;
  const MaterialProperty<std::vector<Real> > & _fissxs;
  std::vector<MooseVariable *> _vars;
  std::vector<const VariableValue *> _group_fluxes;
};

#endif
