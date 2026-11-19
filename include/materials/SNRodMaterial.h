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

  /// Volume fraction of rod material in element
  Real volumeFraction();

  /// Non-rod material key associated with the group constants to be loaded
  std::string _nonrod_material_key;

  /// Non-rod counterparts of the base-class pointers to the map entries, indexed by XsIndex
  std::vector<std::vector<std::vector<Real>> *> _nonrod_xsec_values;
  std::vector<std::vector<SplineInterpolation> *> _nonrod_spline_interps;
  std::vector<std::vector<MonotoneCubicInterpolation> *> _nonrod_monotone_cubic_interps;
  std::vector<std::vector<LinearInterpolation> *> _nonrod_linear_interps;

  /// Rod height function
  const Function & _rod_height;

  /// Rod cusping correction power factor
  const Real _cusp_power;

  /// Rod cusping correction coefficient
  const Real _cusp_coeff;
};
