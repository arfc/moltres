#ifndef HEATPRECURSORDECAY_H
#define HEATPRECURSORDECAY_H

#include "Kernel.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class HeatPrecursorDecay;

template <>
InputParameters validParams<HeatPrecursorDecay>();

class HeatPrecursorDecay : public Kernel, public ScalarTransportBase
{
public:
  HeatPrecursorDecay(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  unsigned int _heat_group;
  RealVectorValue _decay_heat_const;
};

#endif // HEATPRECURSORDECAY_H