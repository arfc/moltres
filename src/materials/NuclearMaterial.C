#include "NuclearMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

registerMooseObject("MoltresApp", NuclearMaterial);

InputParameters
NuclearMaterial::validParams()
{
  InputParameters params = GenericConstantMaterial::validParams();
  params.addRequiredParam<unsigned int>("num_groups",
                                    "The number of groups the energy spectrum is divided into.");
  params.addRequiredParam<unsigned int>("num_precursor_groups",
                                    "The number of delayed neutron precursor groups.");
  params.addCoupledVar(
      "temperature", 937, "The temperature field for determining group constants.");
  params.addRequiredParam<MooseEnum>("interp_type",
                                     NuclearMaterial::interpTypes(),
                                     "The type of interpolation to perform.");
  params.addParam<bool>(
      "sss2_input", true, "Whether serpent 2 was used to generate the input files.");
  params.set<MooseEnum>("constant_on") = "NONE";
  // the following two lines esentially make the two parameters optional
  params.set<std::vector<std::string>>("prop_names") = std::vector<std::string>();
  params.set<std::vector<Real>>("prop_values") = std::vector<Real>();
  return params;
}

NuclearMaterial::NuclearMaterial(const InputParameters & parameters)
  : GenericConstantMaterial(parameters),
    _temperature(coupledValue("temperature")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _remxs(declareProperty<std::vector<Real>>("remxs")),
    _fissxs(declareProperty<std::vector<Real>>("fissxs")),
    _nsf(declareProperty<std::vector<Real>>("nsf")),
    _fisse(declareProperty<std::vector<Real>>("fisse")),
    _diffcoef(declareProperty<std::vector<Real>>("diffcoef")),
    _recipvel(declareProperty<std::vector<Real>>("recipvel")),
    _chi_t(declareProperty<std::vector<Real>>("chi_t")),
    _chi_p(declareProperty<std::vector<Real>>("chi_p")),
    _chi_d(declareProperty<std::vector<Real>>("chi_d")),
    _gtransfxs(declareProperty<std::vector<Real>>("gtransfxs")),
    _beta_eff(declareProperty<std::vector<Real>>("beta_eff")),
    _beta(declareProperty<Real>("beta")),
    _decay_constant(declareProperty<std::vector<Real>>("decay_constant")),

    _d_remxs_d_temp(declareProperty<std::vector<Real>>("d_remxs_d_temp")),
    _d_fissxs_d_temp(declareProperty<std::vector<Real>>("d_fissxs_d_temp")),
    _d_nsf_d_temp(declareProperty<std::vector<Real>>("d_nsf_d_temp")),
    _d_fisse_d_temp(declareProperty<std::vector<Real>>("d_fisse_d_temp")),
    _d_diffcoef_d_temp(declareProperty<std::vector<Real>>("d_diffcoef_d_temp")),
    _d_recipvel_d_temp(declareProperty<std::vector<Real>>("d_recipvel_d_temp")),
    _d_chi_t_d_temp(declareProperty<std::vector<Real>>("d_chi_t_d_temp")),
    _d_chi_p_d_temp(declareProperty<std::vector<Real>>("d_chi_p_d_temp")),
    _d_chi_d_d_temp(declareProperty<std::vector<Real>>("d_chi_d_d_temp")),
    _d_gtransfxs_d_temp(declareProperty<std::vector<Real>>("d_gtransfxs_d_temp")),
    _d_beta_eff_d_temp(declareProperty<std::vector<Real>>("d_beta_eff_d_temp")),
    _d_beta_d_temp(declareProperty<Real>("d_beta_d_temp")),
    _d_decay_constant_d_temp(declareProperty<std::vector<Real>>("d_decay_constant_d_temp")),
    _interp_type(getParam<MooseEnum>("interp_type"))
{
  auto n = _xsec_names.size();
  for (decltype(n) j = 0; j < n; ++j)
  {
    if (_xsec_names[j].compare("GTRANSFXS") == 0)
      _vec_lengths[_xsec_names[j]] = _num_groups * _num_groups;
    else if (_xsec_names[j].compare("BETA_EFF") == 0 || _xsec_names[j].compare("DECAY_CONSTANT") == 0)
      _vec_lengths[_xsec_names[j]] = _num_precursor_groups;
    else
      _vec_lengths[_xsec_names[j]] = _num_groups;
  }
}

void
NuclearMaterial::dummyComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _xsec_map["REMXS"][i][0];
    _fissxs[_qp][i] = _xsec_map["FISSXS"][i][0];
    _nsf[_qp][i] = _xsec_map["NSF"][i][0];
    _fisse[_qp][i] = _xsec_map["FISSE"][i][0] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = _xsec_map["DIFFCOEF"][i][0];
    _recipvel[_qp][i] = _xsec_map["RECIPVEL"][i][0];
    _chi_t[_qp][i] = _xsec_map["CHI_T"][i][0];
    _chi_p[_qp][i] = _xsec_map["CHI_P"][i][0];
    _chi_d[_qp][i] = _xsec_map["CHI_D"][i][0];
    _d_remxs_d_temp[_qp][i] = _xsec_map["REMXS"][i][0];
    _d_fissxs_d_temp[_qp][i] = _xsec_map["FISSXS"][i][0];
    _d_nsf_d_temp[_qp][i] = _xsec_map["NSF"][i][0];
    _d_fisse_d_temp[_qp][i] = _xsec_map["FISSE"][i][0] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = _xsec_map["DIFFCOEF"][i][0];
    _d_recipvel_d_temp[_qp][i] = _xsec_map["RECIPVEL"][i][0];
    _d_chi_t_d_temp[_qp][i] = _xsec_map["CHI_T"][i][0];
    _d_chi_p_d_temp[_qp][i] = _xsec_map["CHI_P"][i][0];
    _d_chi_d_d_temp[_qp][i] = _xsec_map["CHI_D"][i][0];
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] = _xsec_map["GTRANSFXS"][i][0];
    _d_gtransfxs_d_temp[_qp][i] = _xsec_map["GTRANSFXS"][i][0];
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = _xsec_map["BETA_EFF"][i][0];
    _d_beta_eff_d_temp[_qp][i] = _xsec_map["BETA_EFF"][i][0];
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] = _xsec_map["DECAY_CONSTANT"][i][0];
    _d_decay_constant_d_temp[_qp][i] = _xsec_map["DECAY_CONSTANT"][i][0];
  }
}

void
NuclearMaterial::splineComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _xsec_spline_interpolators["REMXS"][i].sample(_temperature[_qp]);
    _fissxs[_qp][i] = _xsec_spline_interpolators["FISSXS"][i].sample(_temperature[_qp]);
    _nsf[_qp][i] = _xsec_spline_interpolators["NSF"][i].sample(_temperature[_qp]);
    _fisse[_qp][i] = _xsec_spline_interpolators["FISSE"][i].sample(_temperature[_qp]) * 1e6 *
                     1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = _xsec_spline_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
    _recipvel[_qp][i] = _xsec_spline_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
    _chi_t[_qp][i] = _xsec_spline_interpolators["CHI_T"][i].sample(_temperature[_qp]);
    _chi_p[_qp][i] = _xsec_spline_interpolators["CHI_P"][i].sample(_temperature[_qp]);
    _chi_d[_qp][i] = _xsec_spline_interpolators["CHI_D"][i].sample(_temperature[_qp]);
    _d_remxs_d_temp[_qp][i] =
        _xsec_spline_interpolators["REMXS"][i].sampleDerivative(_temperature[_qp]);
    _d_fissxs_d_temp[_qp][i] =
        _xsec_spline_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]);
    _d_nsf_d_temp[_qp][i] =
        _xsec_spline_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]);
    _d_fisse_d_temp[_qp][i] =
        _xsec_spline_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]) * 1e6 *
        1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] =
        _xsec_spline_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]);
    _d_recipvel_d_temp[_qp][i] =
        _xsec_spline_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_t_d_temp[_qp][i] =
        _xsec_spline_interpolators["CHI_T"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_p_d_temp[_qp][i] =
        _xsec_spline_interpolators["CHI_P"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_d_d_temp[_qp][i] =
        _xsec_spline_interpolators["CHI_D"][i].sampleDerivative(_temperature[_qp]);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] = _xsec_spline_interpolators["GTRANSFXS"][i].sample(_temperature[_qp]);
    _d_gtransfxs_d_temp[_qp][i] =
        _xsec_spline_interpolators["GTRANSFXS"][i].sampleDerivative(_temperature[_qp]);
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = _xsec_spline_interpolators["BETA_EFF"][i].sample(_temperature[_qp]);
    _d_beta_eff_d_temp[_qp][i] =
        _xsec_spline_interpolators["BETA_EFF"][i].sampleDerivative(_temperature[_qp]);
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] =
        _xsec_spline_interpolators["DECAY_CONSTANT"][i].sample(_temperature[_qp]);
    _d_decay_constant_d_temp[_qp][i] =
        _xsec_spline_interpolators["DECAY_CONSTANT"][i].sampleDerivative(_temperature[_qp]);
  }
}

void
NuclearMaterial::monotoneCubicComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _xsec_monotone_cubic_interpolators["REMXS"][i].sample(_temperature[_qp]);
    _fissxs[_qp][i] = _xsec_monotone_cubic_interpolators["FISSXS"][i].sample(_temperature[_qp]);
    _nsf[_qp][i] = _xsec_monotone_cubic_interpolators["NSF"][i].sample(_temperature[_qp]);
    _fisse[_qp][i] = _xsec_monotone_cubic_interpolators["FISSE"][i].sample(_temperature[_qp]) *
                     1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = _xsec_monotone_cubic_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
    _recipvel[_qp][i] = _xsec_monotone_cubic_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
    _chi_t[_qp][i] = _xsec_monotone_cubic_interpolators["CHI_T"][i].sample(_temperature[_qp]);
    _chi_p[_qp][i] = _xsec_monotone_cubic_interpolators["CHI_P"][i].sample(_temperature[_qp]);
    _chi_d[_qp][i] = _xsec_monotone_cubic_interpolators["CHI_D"][i].sample(_temperature[_qp]);
    _d_remxs_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["REMXS"][i].sampleDerivative(_temperature[_qp]);
    _d_fissxs_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]);
    _d_nsf_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]);
    _d_fisse_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]) * 1e6 *
        1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]);
    _d_recipvel_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_t_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["CHI_T"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_p_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["CHI_P"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_d_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["CHI_D"][i].sampleDerivative(_temperature[_qp]);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] =
        _xsec_monotone_cubic_interpolators["GTRANSFXS"][i].sample(_temperature[_qp]);
    _d_gtransfxs_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["GTRANSFXS"][i].sampleDerivative(_temperature[_qp]);
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = _xsec_monotone_cubic_interpolators["BETA_EFF"][i].sample(_temperature[_qp]);
    _d_beta_eff_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["BETA_EFF"][i].sampleDerivative(_temperature[_qp]);
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] =
        _xsec_monotone_cubic_interpolators["DECAY_CONSTANT"][i].sample(_temperature[_qp]);
    _d_decay_constant_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["DECAY_CONSTANT"][i].sampleDerivative(_temperature[_qp]);
  }
}

void
NuclearMaterial::linearComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _xsec_linear_interpolators["REMXS"][i].sample(_temperature[_qp]);
    _fissxs[_qp][i] = _xsec_linear_interpolators["FISSXS"][i].sample(_temperature[_qp]);
    _nsf[_qp][i] = _xsec_linear_interpolators["NSF"][i].sample(_temperature[_qp]);
    _fisse[_qp][i] = _xsec_linear_interpolators["FISSE"][i].sample(_temperature[_qp]) * 1e6 *
                     1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = _xsec_linear_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
    _recipvel[_qp][i] = _xsec_linear_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
    _chi_t[_qp][i] = _xsec_linear_interpolators["CHI_T"][i].sample(_temperature[_qp]);
    _chi_p[_qp][i] = _xsec_linear_interpolators["CHI_P"][i].sample(_temperature[_qp]);
    _chi_d[_qp][i] = _xsec_linear_interpolators["CHI_D"][i].sample(_temperature[_qp]);
    _d_remxs_d_temp[_qp][i] =
        _xsec_linear_interpolators["REMXS"][i].sampleDerivative(_temperature[_qp]);
    _d_fissxs_d_temp[_qp][i] =
        _xsec_linear_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]);
    _d_nsf_d_temp[_qp][i] =
        _xsec_linear_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]);
    _d_fisse_d_temp[_qp][i] =
        _xsec_linear_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]) * 1e6 *
        1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] =
        _xsec_linear_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]);
    _d_recipvel_d_temp[_qp][i] =
        _xsec_linear_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_t_d_temp[_qp][i] =
        _xsec_linear_interpolators["CHI_T"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_p_d_temp[_qp][i] =
        _xsec_linear_interpolators["CHI_P"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_d_d_temp[_qp][i] =
        _xsec_linear_interpolators["CHI_D"][i].sampleDerivative(_temperature[_qp]);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] = _xsec_linear_interpolators["GTRANSFXS"][i].sample(_temperature[_qp]);
    _d_gtransfxs_d_temp[_qp][i] =
        _xsec_linear_interpolators["GTRANSFXS"][i].sampleDerivative(_temperature[_qp]);
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = _xsec_linear_interpolators["BETA_EFF"][i].sample(_temperature[_qp]);
    _d_beta_eff_d_temp[_qp][i] =
        _xsec_linear_interpolators["BETA_EFF"][i].sampleDerivative(_temperature[_qp]);
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] =
        _xsec_linear_interpolators["DECAY_CONSTANT"][i].sample(_temperature[_qp]);
    _d_decay_constant_d_temp[_qp][i] =
        _xsec_linear_interpolators["DECAY_CONSTANT"][i].sampleDerivative(_temperature[_qp]);
  }
}

void
NuclearMaterial::preComputeQpProperties()
{
  for (unsigned int i = 0; i < _num_props; i++)
    (*_properties[i])[_qp] = _prop_values[i];
  _remxs[_qp].resize(_num_groups);
  _fissxs[_qp].resize(_num_groups);
  _nsf[_qp].resize(_num_groups);
  _fisse[_qp].resize(_num_groups);
  _diffcoef[_qp].resize(_num_groups);
  _recipvel[_qp].resize(_num_groups);
  _chi_t[_qp].resize(_num_groups);
  _chi_p[_qp].resize(_num_groups);
  _chi_d[_qp].resize(_num_groups);

  _gtransfxs[_qp].resize(_num_groups * _num_groups);

  _beta_eff[_qp].resize(_num_precursor_groups);
  _decay_constant[_qp].resize(_num_precursor_groups);

  _d_remxs_d_temp[_qp].resize(_num_groups);
  _d_fissxs_d_temp[_qp].resize(_num_groups);
  _d_nsf_d_temp[_qp].resize(_num_groups);
  _d_fisse_d_temp[_qp].resize(_num_groups);
  _d_diffcoef_d_temp[_qp].resize(_num_groups);
  _d_recipvel_d_temp[_qp].resize(_num_groups);
  _d_chi_t_d_temp[_qp].resize(_num_groups);
  _d_chi_p_d_temp[_qp].resize(_num_groups);
  _d_chi_d_d_temp[_qp].resize(_num_groups);

  _d_gtransfxs_d_temp[_qp].resize(_num_groups * _num_groups);

  _d_beta_eff_d_temp[_qp].resize(_num_precursor_groups);
  _d_decay_constant_d_temp[_qp].resize(_num_precursor_groups);
}
