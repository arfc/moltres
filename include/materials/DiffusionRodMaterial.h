#pragma once

#include "MoltresJsonMaterial.h"

class DiffusionRodMaterial : public MoltresJsonMaterial
{
public:
  DiffusionRodMaterial(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  void Construct(nlohmann::json xs_root);
  virtual void dummyComputeQpProperties() override;
  virtual void splineComputeQpProperties() override;
  virtual void monotoneCubicComputeQpProperties() override;
  virtual void linearComputeQpProperties() override;

  /// Volume fraction of rod material in element
  Real volumeFraction();

  /// Non-rod material key associated with the group constants to be loaded
  std::string _nonrod_material_key;

  /// Rod height function
  const Function & _rod_height;

  /// Rod cusping correction power factor
  const Real _cusp_power;

  /// Rod cusping correction coefficient
  const Real _cusp_coeff;
};
