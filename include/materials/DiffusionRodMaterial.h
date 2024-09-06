#pragma once

#include "MoltresJsonMaterial.h"
#include "NeighborCoupleable.h"

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
  std::vector<Real> fluxVolumeFraction();

  // Non-rod material key associated with the group constants to be loaded
  std::string _nonrod_material_key;

  // Rod height function
  const Function & _rod_height;
  std::vector<const VariableValue *> _group_fluxes;
};
