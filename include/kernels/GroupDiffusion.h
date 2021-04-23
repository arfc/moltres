#pragma once

#include "Kernel.h"
#include "ScalarTransportBase.h"

class GroupDiffusion : public Kernel, public ScalarTransportBase
{
public:
  GroupDiffusion(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<std::vector<Real>> & _diffcoef;
  const MaterialProperty<std::vector<Real>> & _d_diffcoef_d_temp;
  unsigned int _group;
  unsigned int _temp_id;
};
