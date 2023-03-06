#pragma once

#include "NuclearMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"
#include "MonotoneCubicInterpolation.h"
#include "LinearInterpolation.h"

class GenericMoltresMaterial : public NuclearMaterial
{
public:
  GenericMoltresMaterial(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  void Construct(std::string & property_tables_root);
  void bicubicSplineConstruct(std::string & property_tables_root,
                              const InputParameters & parameters);
  void leastSquaresConstruct(std::string & property_tables_root);
  virtual void computeQpProperties() override;
  virtual void fuelBicubic();
  virtual void moderatorBicubic();
  virtual void bicubicSplineComputeQpProperties();
  virtual void leastSquaresComputeQpProperties();


  const PostprocessorValue & _other_temp;
  const PostprocessorValue & _peak_power_density;
  Real _peak_power_density_set_point;
  Real _controller_gain;

  std::string _material;
  bool _perform_control;
};
