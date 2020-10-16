#ifndef MOLTRESJSONMATERIAL_H_
#define MOLTRESJSONMATERIAL_H_

#include "NuclearMaterial.h"
#include "nlohmann/json.h"

class MoltresJsonMaterial;

template <>
InputParameters validParams<MoltresJsonMaterial>();

class MoltresJsonMaterial : public NuclearMaterial
{
public:
  MoltresJsonMaterial(const InputParameters & parameters);

protected:
  void Construct(nlohmann::json xs_root, std::vector<std::string> xsec_names);
  virtual void computeQpProperties();

  std::string _material_key;
};

#endif // MOLTRESJSONMATERIAL_H
