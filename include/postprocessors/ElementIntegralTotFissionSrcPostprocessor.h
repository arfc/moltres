#ifndef ELEMENTINTEGRALTOTFISSIONSRCPOSTPROCESSOR_H
#define ELEMENTINTEGRALTOTFISSIONSRCPOSTPROCESSOR_H

#include "ElementIntegralPostprocessor.h"
#include "MooseVariableInterface.h"

//Forward Declarations
class ElementIntegralTotFissionSrcPostprocessor;

template<>
InputParameters validParams<ElementIntegralTotFissionSrcPostprocessor>();

/**
 * This postprocessor computes a volume integral of the specified variable.
 *
 * Note that specializations of this integral are possible by deriving from this
 * class and overriding computeQpIntegral().
 */
class ElementIntegralTotFissionSrcPostprocessor :
  public ElementIntegralPostprocessor
  /* public MooseVariableInterface */
{
public:
  ElementIntegralTotFissionSrcPostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

  int _num_groups;
  const MaterialProperty<std::vector<Real> > & _nsf;
  std::vector<MooseVariable *> _vars;
  std::vector<const VariableValue *> _group_fluxes;
};

#endif
