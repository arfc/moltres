#ifndef GENERICMOLTRESMATERIAL_H_
#define GENERICMOLTRESMATERIAL_H_

#include "GenericConstantMaterial.h"
#include "SplineInterpolation.h"
#include "BicubicSplineInterpolation.h"
#include "MonotoneCubicInterpolation.h"
#include "LinearInterpolation.h"

class GenericMoltresMaterial;

template <>
InputParameters validParams<GenericMoltresMaterial>();

/**
 * This class hinges on the `interp_type` chosen by the user. The user can choose from the
 * following interpolation types:
 *
 * "normal interpolations"
 * bicubic_spline : used when cross section is a function of two different temperatures (say fuel
 *                  and moderator). Performs bicubic spline interpolation. Example input property
 *                  table: property_file_dir/msr2g_Th_U_two_mat_homogenization_fuel_interp_NSF.txt
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
 * least_squares :  user should have performed a linear least squares fit on cross section data
 *                  and should supply interpolation table with each row corresponding to energy
 *                  group. Each row should have two columns corresponding to 'a' and 'b' where
 *                  xsec(T) = a * T + b
 */
class GenericMoltresMaterial : public GenericConstantMaterial
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
  virtual void dummyComputeQpProperties();
  virtual void splineComputeQpProperties();
  virtual void monotoneCubicComputeQpProperties();
  virtual void linearComputeQpProperties();
  virtual void fuelBicubic();
  virtual void moderatorBicubic();
  virtual void bicubicSplineComputeQpProperties();
  virtual void leastSquaresComputeQpProperties();
  virtual void computeQpProperties();

  const VariableValue & _temperature;

  MaterialProperty<std::vector<Real>> & _remxs;
  MaterialProperty<std::vector<Real>> & _fissxs;
  MaterialProperty<std::vector<Real>> & _nsf;
  MaterialProperty<std::vector<Real>> & _fisse;
  MaterialProperty<std::vector<Real>> & _diffcoef;
  MaterialProperty<std::vector<Real>> & _recipvel;
  MaterialProperty<std::vector<Real>> & _chi;
  MaterialProperty<std::vector<Real>> & _chi_d;
  MaterialProperty<std::vector<Real>> & _gtransfxs;
  MaterialProperty<std::vector<Real>> & _beta_eff;
  MaterialProperty<Real> & _beta;
  MaterialProperty<std::vector<Real>> & _decay_constant;
  MaterialProperty<std::vector<Real>> & _d_remxs_d_temp;
  MaterialProperty<std::vector<Real>> & _d_fissxs_d_temp;
  MaterialProperty<std::vector<Real>> & _d_nsf_d_temp;
  MaterialProperty<std::vector<Real>> & _d_fisse_d_temp;
  MaterialProperty<std::vector<Real>> & _d_diffcoef_d_temp;
  MaterialProperty<std::vector<Real>> & _d_recipvel_d_temp;
  MaterialProperty<std::vector<Real>> & _d_chi_d_temp;
  MaterialProperty<std::vector<Real>> & _d_chi_d_d_temp;
  MaterialProperty<std::vector<Real>> & _d_gtransfxs_d_temp;
  MaterialProperty<std::vector<Real>> & _d_beta_eff_d_temp;
  MaterialProperty<Real> & _d_beta_d_temp;
  MaterialProperty<std::vector<Real>> & _d_decay_constant_d_temp;

  MooseEnum _interp_type;
  const PostprocessorValue & _other_temp;
  const PostprocessorValue & _peak_power_density;
  Real _peak_power_density_set_point;
  Real _controller_gain;

  unsigned _num_groups;
  unsigned _num_precursor_groups;
  std::map<std::string, std::vector<Real>> _xsec_map;
  std::map<std::string, std::vector<SplineInterpolation>> _xsec_spline_interpolators;
  std::map<std::string, std::vector<MonotoneCubicInterpolation>> _xsec_monotone_cubic_interpolators;
  std::map<std::string, std::vector<LinearInterpolation>> _xsec_linear_interpolators;
  std::map<std::string, std::vector<BicubicSplineInterpolation>> _xsec_bicubic_spline_interpolators;
  std::map<std::string, int> _vec_lengths;
  std::map<std::string, std::string> _file_map;
  std::string _material;

  std::vector<std::vector<Real>> _flux_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _remxs_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _fissxs_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _nubar_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _nsf_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _fisse_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _diffcoeff_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _recipvel_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _chi_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _chi_d_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _gtransfxs_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _beta_eff_consts = std::vector<std::vector<Real>>(2);
  std::vector<std::vector<Real>> _decay_constants_consts = std::vector<std::vector<Real>>(2);

  bool _perform_control;
};

#endif // GENERICMOLTRESMATERIAL_H
