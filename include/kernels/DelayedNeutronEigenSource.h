#ifndef DELAYEDNEUTRONEIGENSOURCE_H
#define DELAYEDNEUTRONEIGENSOURCE_H

#include "EigenKernel.h"
#include "ScalarTransportBase.h"

// Forward Declarations
class DelayedNeutronEigenSource;

template <>
InputParameters validParams<DelayedNeutronEigenSource>();

class DelayedNeutronEigenSource : public EigenKernel, public ScalarTransportBase
{
public:
  DelayedNeutronEigenSource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _decay_constant;
  const MaterialProperty<std::vector<Real>> & _d_decay_constant_d_temp;
  unsigned int _group;
  const MaterialProperty<std::vector<Real>> &  _chi_d;
  // todo add the jacobian (it's going to be negligible tho)

  unsigned int _num_precursor_groups;
  unsigned int _temp_id;
  const VariableValue & _temp;
  std::vector<const VariableValue *> _pre_concs;
  std::vector<unsigned int> _pre_ids;
};

#endif // DELAYEDNEUTRONEIGENSOURCE_H
