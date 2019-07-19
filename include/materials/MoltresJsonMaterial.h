#ifndef MOLTRESJSONMATERIAL_H_
#define MOLTRESJSONMATERIAL_H_

#include "NuclearMaterial.h"
#include "json.h"

class MoltresJsonMaterial;

template <>
InputParameters validParams<MoltresJsonMaterial>();

class MoltresJsonMaterial : public NuclearMaterial
{
public:
  MoltresJsonMaterial(const InputParameters & parameters);

protected:
  void Construct(moosecontrib::Json::Value xs_root, std::vector<std::string> xsec_names);
  virtual void computeQpProperties();

  std::string _material_key;
};

#endif // MOLTRESJSONMATERIAL_H
