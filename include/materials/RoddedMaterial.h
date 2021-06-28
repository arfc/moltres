#pragma once

#include "GenericMoltresMaterial.h"
#include "ODEKernel.h"

/**
 * This class is meant to simulate a piece of material with a rod in it.
 * In the future, it is expected that better, more accurate code will be written;
 * this is a proof of concept to get pretty plots.
 *
 * The idea is to load material properties like any other moltres material,
 * but if the quadrature point is above control rod position, a greatly increased absorbtion
 * XS is returned.
 *
 * rodPosition : the scalar variable giving rod position
 * absorb_factor : how much to scale up REMXS by
 *
 */
class RoddedMaterial : public GenericMoltresMaterial
{
public:
  RoddedMaterial(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  void splineConstruct(std::string & property_tables_root, std::vector<std::string> xsec_names);
  virtual void computeQpProperties() override;
  virtual void computeSplineAbsorbingQpProperties();

  Real _absorb_factor;
  const VariableValue & _rod_pos;
  MooseEnum _rod_dim;
};
