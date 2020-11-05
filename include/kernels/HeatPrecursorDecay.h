#ifndef HEATPRECURSORDECAY_H
#define HEATPRECURSORDECAY_H

#include "Kernel.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class HeatPrecursorDecay;

template <>
InputParameters validParams<HeatPrecursorDecay>();

/**
 * This class computes the residual and Jacobian contributions for the
 * decay heat precursor decay term in the decay heat precursor
 * equation.
 */
class HeatPrecursorDecay : public Kernel, public ScalarTransportBase
{
public:
  HeatPrecursorDecay(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  unsigned int _heat_group;
  std::vector<Real> _decay_heat_const;
};

#endif // HEATPRECURSORDECAY_H
