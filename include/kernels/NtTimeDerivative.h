#pragma once

#include "ScalarTransportTimeDerivative.h"

class NtTimeDerivative : public ScalarTransportTimeDerivative
{
public:
  NtTimeDerivative(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _recipvel;
  const MaterialProperty<std::vector<Real>> & _d_recipvel_d_temp;

  /// Neutron group number
  const unsigned int _group;

  /// Temperature variable ID
  const unsigned int _temp_id;
};
