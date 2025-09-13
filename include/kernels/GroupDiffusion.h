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

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _diffcoef;
  const MaterialProperty<std::vector<Real>> & _d_diffcoef_d_temp;

  /// Neutron group number
  const unsigned int _group;

  /// Temperature variable ID
  const unsigned int _temp_id;

  /// Whether to set a diffusion coefficient limit
  const bool _set_limit;

  /// Diffusion coefficient limit value
  const Real _limit;
};
