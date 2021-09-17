#include "NuclearMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

registerMooseObject("MoltresApp", NuclearMaterial);

InputParameters
NuclearMaterial::validParams()
{
  InputParameters params = GenericConstantMaterial::validParams();
  params.addRequiredParam<unsigned>("num_groups",
                                    "The number of groups the energy spectrum is divided into.");
  params.addRequiredParam<unsigned>("num_precursor_groups",
                                    "The number of delayed neutron precursor groups.");
  params.addCoupledVar(
      "temperature", 937, "The temperature field for determining group constants.");
  params.addRequiredParam<MooseEnum>("interp_type",
                                     GenericMoltresMaterial::interpTypes(),
                                     "The type of interpolation to perform.");
  params.addParam<bool>(
      "sss2_input", true, "Whether serpent 2 was used to generate the input files.");
  params.set<MooseEnum>("constant_on") = "NONE";
  return params;
}

NuclearMaterial::NuclearMaterial(const InputParameters & parameters)
  : GenericConstantMaterial(parameters),
    _temperature(coupledValue("temperature")),
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
  std::vector<std::string> xsec_names{"REMXS",
                                      "FISSXS",
                                      "NSF",
                                      "FISSE",
                                      "DIFFCOEF",
                                      "RECIPVEL",
                                      "CHI_T",
                                      "CHI_P",
                                      "CHI_D",
                                      "GTRANSFXS",
                                      "BETA_EFF",
                                      "DECAY_CONSTANT"};

  _num_groups = getParam<unsigned>("num_groups");
  _num_precursor_groups = getParam<unsigned>("num_precursor_groups");

  auto n = xsec_names.size();
  for (decltype(n) j = 0; j < n; ++j)
  {
    if (xsec_names[j].compare("GTRANSFXS") == 0)
      _vec_lengths[xsec_names[j]] = _num_groups * _num_groups;
    else if (xsec_names[j].compare("BETA_EFF") == 0 || xsec_names[j].compare("DECAY_CONSTANT") == 0)
      _vec_lengths[xsec_names[j]] = _num_precursor_groups;
    else
      _vec_lengths[xsec_names[j]] = _num_groups;
  }
  _file_map["REMXS"] = "REMXS";
  _file_map["NSF"] = "NSF";
  _file_map["DIFFCOEF"] = "DIFFCOEF";
  _file_map["BETA_EFF"] = "BETA_EFF";
  _file_map["FLUX"] = "FLUX";
  _file_map["FISSXS"] = "FISSXS";
  _file_map["FISSE"] = "FISSE";
  _file_map["RECIPVEL"] = "RECIPVEL";
  _file_map["CHI_T"] = "CHI_T";
  _file_map["CHI_P"] = "CHI_P";
  _file_map["CHI_D"] = "CHI_D";
  _file_map["GTRANSFXS"] = "GTRANSFXS";
  _file_map["DECAY_CONSTANT"] = "DECAY_CONSTANT";
}

void
NuclearMaterial::dummyComputeQpProperties()
{
  auto dummy_temp = _XsTemperature[0];
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _xsec_linear_interpolators["REMXS"][i].sample(dummy_temp);
    _fissxs[_qp][i] = _xsec_linear_interpolators["FISSXS"][i].sample(dummy_temp);
    _nsf[_qp][i] = _xsec_linear_interpolators["NSF"][i].sample(dummy_temp);
    _fisse[_qp][i] = _xsec_linear_interpolators["FISSE"][i].sample(dummy_temp) * 1e6 *
                     1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = _xsec_linear_interpolators["DIFFCOEF"][i].sample(dummy_temp);
    _recipvel[_qp][i] = _xsec_linear_interpolators["RECIPVEL"][i].sample(dummy_temp);
    _chi_t[_qp][i] = _xsec_linear_interpolators["CHI_T"][i].sample(dummy_temp);
    _chi_p[_qp][i] = _xsec_linear_interpolators["CHI_P"][i].sample(dummy_temp);
    _chi_d[_qp][i] = _xsec_linear_interpolators["CHI_D"][i].sample(dummy_temp);
    _d_remxs_d_temp[_qp][i] = _xsec_linear_interpolators["REMXS"][i].sampleDerivative(dummy_temp);
    _d_fissxs_d_temp[_qp][i] = _xsec_linear_interpolators["FISSXS"][i].sampleDerivative(dummy_temp);
    _d_nsf_d_temp[_qp][i] = _xsec_linear_interpolators["NSF"][i].sampleDerivative(dummy_temp);
    _d_fisse_d_temp[_qp][i] = _xsec_linear_interpolators["FISSE"][i].sampleDerivative(dummy_temp) *
                              1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] =
        _xsec_linear_interpolators["DIFFCOEF"][i].sampleDerivative(dummy_temp);
    _d_recipvel_d_temp[_qp][i] =
        _xsec_linear_interpolators["RECIPVEL"][i].sampleDerivative(dummy_temp);
    _d_chi_t_d_temp[_qp][i] = _xsec_linear_interpolators["CHI_T"][i].sampleDerivative(dummy_temp);
    _d_chi_p_d_temp[_qp][i] = _xsec_linear_interpolators["CHI_P"][i].sampleDerivative(dummy_temp);
    _d_chi_d_d_temp[_qp][i] = _xsec_linear_interpolators["CHI_D"][i].sampleDerivative(dummy_temp);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] = _xsec_linear_interpolators["GTRANSFXS"][i].sample(dummy_temp);
    _d_gtransfxs_d_temp[_qp][i] =
        _xsec_linear_interpolators["GTRANSFXS"][i].sampleDerivative(dummy_temp);
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = _xsec_linear_interpolators["BETA_EFF"][i].sample(dummy_temp);
    _d_beta_eff_d_temp[_qp][i] =
        _xsec_linear_interpolators["BETA_EFF"][i].sampleDerivative(dummy_temp);
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] = _xsec_linear_interpolators["DECAY_CONSTANT"][i].sample(dummy_temp);
    _d_decay_constant_d_temp[_qp][i] =
        _xsec_linear_interpolators["DECAY_CONSTANT"][i].sampleDerivative(dummy_temp);
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
