#ifndef CONSTANTDIFFERENCEAUX_H
#define CONSTANTDIFFERENCEAUX_H

#include "AuxKernel.h"

// Forward Declarations
class ConstantDifferenceAux;

template <>
InputParameters validParams<ConstantDifferenceAux>();

/**
 * The original intention of this aux kernel is to compute a delta T about some
 * nominal temperature used in the Boussinesq approximation for buoyancy in
 * incompressible Navier Stokes. The nominal temperature may need to change over
 * time in a simulation so that the relative size of the residual contributed by
 * the INSBoussinesqBodyForce stays around the same size as the other kernels.
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
