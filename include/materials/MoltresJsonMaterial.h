#pragma once

#include "NuclearMaterial.h"
#include "nlohmann/json.h"

class MoltresJsonMaterial : public NuclearMaterial
{
public:
  MoltresJsonMaterial(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  void Construct(nlohmann::json xs_root);
  virtual void computeQpProperties() override;

  // Vector of group constants to be loaded
  std::vector<std::string> _group_consts;

  // Material associated with the group constants to be loaded
  std::string _material_key;
};
