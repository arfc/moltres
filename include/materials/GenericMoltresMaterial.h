#ifndef GENERICMOLTRESMATERIAL_H_
#define GENERICMOLTRESMATERIAL_H_

#include "NuclearMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"
#include "MonotoneCubicInterpolation.h"
#include "LinearInterpolation.h"

class GenericMoltresMaterial;

template <>
InputParameters validParams<GenericMoltresMaterial>();

class GenericMoltresMaterial : public NuclearMaterial
{
public:
  GenericMoltresMaterial(const InputParameters & parameters);

protected:
  void dummyConstruct(std::string & property_tables_root, std::vector<std::string> xsec_names);
  void splineConstruct(std::string & property_tables_root, std::vector<std::string> xsec_names);
  void monotoneCubicConstruct(std::string & property_tables_root,
                              std::vector<std::string> xsec_names);
  void linearConstruct(std::string & property_tables_root, std::vector<std::string> xsec_names);
  void bicubicSplineConstruct(std::string & property_tables_root,
                              std::vector<std::string> xsec_names,
                              const InputParameters & parameters);
  void leastSquaresConstruct(std::string & property_tables_root,
                             std::vector<std::string> xsec_names);
  virtual void computeQpProperties();
  virtual void dummyComputeQpProperties();
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

#endif // GENERICMOLTRESMATERIAL_H
