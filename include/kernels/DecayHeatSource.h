#ifndef DECAYHEATSOURCE_H
#define DECAYHEATSOURCE_H

#include "Kernel.h"
#include "ScalarTranportBase.h"

// Forward Declarations
class DecayHeatSource;

template <>
InputParameters validParams<DecayHeatSource>();

class DecayHeatSource : public Kernel, public ScalarTransportBase
{
public:
  DecayHeatSource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  unsigned int _num_heat_groups;
  std::vector<Real> _decay_heat_const;
  std::vector<const VariableValue *> _heat_concs;
  std::vector<unsigned int> _heat_ids;
};

#endif // DECAYHEATSOURCE_H