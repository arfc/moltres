#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class SigmaR : public Kernel, public ScalarTransportBase
{
public:
  SigmaR(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _remxs;
  const MaterialProperty<std::vector<Real>> & _d_remxs_d_temp;
  const unsigned int _group;
  const unsigned int _temp_id;
};
