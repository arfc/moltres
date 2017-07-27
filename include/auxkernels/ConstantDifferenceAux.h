#ifndef CONSTANTDIFFERENCEAUX_H
#define CONSTANTDIFFERENCEAUX_H

#include "AuxKernel.h"

// Forward Declarations
class ConstantDifferenceAux;

template <>
InputParameters validParams<ConstantDifferenceAux>();

/**
 * Coupled auxiliary value, subtracts out some value
 */
class ConstantDifferenceAux : public AuxKernel
{
public:
  ConstantDifferenceAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  const VariableValue & _variable;
  const Real & _constant;
};

#endif // CONSTANTDIFFERENCEAUX_H
