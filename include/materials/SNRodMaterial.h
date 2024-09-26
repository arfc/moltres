#pragma once

#include "MoltresSNMaterial.h"

class SNRodMaterial : public MoltresSNMaterial
{
public:
  SNRodMaterial(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  void Construct(nlohmann::json xs_root);
  virtual void dummyComputeQpProperties() override;
  virtual void splineComputeQpProperties() override;
  virtual void monotoneCubicComputeQpProperties() override;
  virtual void linearComputeQpProperties() override;
  virtual void computeQpProperties() override;
  Real volumeFraction();

  // Non-rod material key associated with the group constants to be loaded
  std::string _nonrod_material_key;

  // Rod height function
  const Function & _rod_height;

  // Rod cusping correction factor
  const Real _cusp_correction;
};
