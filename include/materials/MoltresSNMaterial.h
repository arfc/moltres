#pragma once

#include "GenericConstantMaterial.h"
#include "SplineInterpolation.h"
#include "MonotoneCubicInterpolation.h"
#include "LinearInterpolation.h"
#include "nlohmann/json.h"

/**
 * This class hinges on the `interp_type` chosen by the user. The user can choose from the
 * following interpolation types:
 *
 * "normal interpolations"
 * spline :         cubic spline interpolation for single temperature. Example input property
 *                  table: property_file_dir/newt_msre_fuel_NSF.txt
 * monotone_cubic : monotone cubic interpolation for single temperature. Same input property table
 *                  structure as spline
 * linear :         linear interpolation for single temperature. Same input property table
 *                  structure as spline
 *
 * "special interpolations"
 * none :           no interpolation is done. The cross section value for each group is read from
 *                  the first temperature row in the interpolation table
 */
class MoltresSNMaterial : public GenericConstantMaterial
{
public:
  MoltresSNMaterial(const InputParameters & parameters);

  static InputParameters validParams();

  // correspond to descriptions above
  enum INTERPOLATOR
  {
    SPLINE,
    MONOTONE_CUBIC,
    LINEAR,
    NONE
  };

  // returns a MooseEnum corresponding to the above enum
  static MooseEnum interpTypes()
  {
    return MooseEnum("spline=0 monotone_cubic=1 linear=2 none=3");
  }

protected:
  virtual void dummyComputeQpProperties();
  virtual void splineComputeQpProperties();
  virtual void monotoneCubicComputeQpProperties();
  virtual void linearComputeQpProperties();
  virtual void preComputeQpProperties();
  void Construct(nlohmann::json xs_root);
  virtual void computeQpProperties() override;

  const VariableValue & _temperature;

  // Number of neutron groups
  unsigned int _num_groups;

  // Number of precursor groups
  unsigned int _num_precursor_groups;

  // Group constant MaterialProperty(s)
  MaterialProperty<std::vector<Real>> & _totxs;
  MaterialProperty<std::vector<Real>> & _fissxs;
  MaterialProperty<std::vector<Real>> & _nsf;
  MaterialProperty<std::vector<Real>> & _fisse;
  MaterialProperty<std::vector<Real>> & _recipvel;
  MaterialProperty<std::vector<Real>> & _chi_t;
  MaterialProperty<std::vector<Real>> & _chi_p;
  MaterialProperty<std::vector<Real>> & _chi_d;
  MaterialProperty<std::vector<Real>> & _scatter;
  MaterialProperty<std::vector<Real>> & _beta_eff;
  MaterialProperty<Real> & _beta;
  MaterialProperty<std::vector<Real>> & _decay_constant;
  MaterialProperty<std::vector<Real>> & _diffcoef;
  MaterialProperty<std::vector<Real>> & _d_totxs_d_temp;
  MaterialProperty<std::vector<Real>> & _d_fissxs_d_temp;
  MaterialProperty<std::vector<Real>> & _d_nsf_d_temp;
  MaterialProperty<std::vector<Real>> & _d_fisse_d_temp;
  MaterialProperty<std::vector<Real>> & _d_recipvel_d_temp;
  MaterialProperty<std::vector<Real>> & _d_chi_t_d_temp;
  MaterialProperty<std::vector<Real>> & _d_chi_p_d_temp;
  MaterialProperty<std::vector<Real>> & _d_chi_d_d_temp;
  MaterialProperty<std::vector<Real>> & _d_scatter_d_temp;
  MaterialProperty<std::vector<Real>> & _d_beta_eff_d_temp;
  MaterialProperty<Real> & _d_beta_d_temp;
  MaterialProperty<std::vector<Real>> & _d_decay_constant_d_temp;
  MaterialProperty<std::vector<Real>> & _d_diffcoef_d_temp;

  // Group constant interpolation type
  MooseEnum _interp_type;

  // Vector of group constants to be loaded
  std::vector<std::string> _group_consts;

  // Material associated with the group constants to be loaded
  std::string _material_key;

  // Discrete ordinate order
  unsigned int _N;

  // Maximum scattering Legendre moment
  unsigned int _L;

  // Void constant for stabilization scheme in voids
  const Real _sigma;

  // Stabilization constant for stabilization scheme in voids
  const Real _c;

  const MooseEnum _h_type;

  MaterialProperty<std::vector<Real>> & _tau_sn;

  // Vector of group constant names
  std::vector<std::string> _xsec_names{"TOTXS",
                                       "FISSXS",
                                       "NSF",
                                       "FISSE",
                                       "RECIPVEL",
                                       "CHI_T",
                                       "CHI_P",
                                       "CHI_D",
                                       "SPN",
                                       "BETA_EFF",
                                       "DECAY_CONSTANT",
                                       "DIFFCOEF"};

  // Map of group constant names to group constant values
  std::map<std::string, std::vector<std::vector<Real>>> _xsec_map;

  // Group constant interpolators
  std::map<std::string, std::vector<SplineInterpolation>> _xsec_spline_interpolators;
  std::map<std::string, std::vector<MonotoneCubicInterpolation>> _xsec_monotone_cubic_interpolators;
  std::map<std::string, std::vector<LinearInterpolation>> _xsec_linear_interpolators;

  // Map of group constant names to number of neutron/precursor groups
  std::map<std::string, int> _vec_lengths;

  // Vector of temperature values
  std::vector<double> _XsTemperature;
};
