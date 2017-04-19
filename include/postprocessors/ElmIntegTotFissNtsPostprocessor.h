#ifndef ELMINTEGTOTFISSNTSPOSTPROCESSOR_H
#define ELMINTEGTOTFISSNTSPOSTPROCESSOR_H

#include "ElementIntegralPostprocessor.h"
#include "MooseVariableInterface.h"

// Forward Declarations
class ElmIntegTotFissNtsPostprocessor;

template <>
InputParameters validParams<ElmIntegTotFissNtsPostprocessor>();

class ElmIntegTotFissNtsPostprocessor : public ElementIntegralPostprocessor
/* public MooseVariableInterface */
{
public:
  ElmIntegTotFissNtsPostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

  int _num_groups;
  const MaterialProperty<std::vector<Real>> & _nsf;
  std::vector<MooseVariable *> _vars;
  std::vector<const VariableValue *> _group_fluxes;
};

#endif
