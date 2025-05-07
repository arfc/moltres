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

  const MaterialProperty<std::vector<Real>> & _recipvel;
  const MaterialProperty<std::vector<Real>> & _d_recipvel_d_temp;
  const unsigned int _group;
  const unsigned int _temp_id;
};
