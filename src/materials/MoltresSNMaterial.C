#include "MoltresSNMaterial.h"
#include "MooseUtils.h"

registerMooseObject("MoltresApp", MoltresSNMaterial);

InputParameters
MoltresSNMaterial::validParams()
{
  InputParameters params = GenericConstantMaterial::validParams();
  params.addRequiredParam<unsigned int>("num_groups",
                                    "The number of groups the energy spectrum is divided into.");
  params.addRequiredParam<unsigned int>("num_precursor_groups",
                                    "The number of delayed neutron precursor groups.");
  params.addCoupledVar(
      "temperature", 937, "The temperature field for determining group constants.");
  params.addRequiredParam<MooseEnum>("interp_type",
                                     MoltresSNMaterial::interpTypes(),
                                     "The type of interpolation to perform.");
  params.set<MooseEnum>("constant_on") = "NONE";
  params.addRequiredParam<std::string>("base_file", "The file containing macroscopic XS.");
  params.addRequiredParam<std::string>("material_key",
                                       "The file key where the macroscopic XS can be found.");
  params.addParam<std::vector<std::string>>(
      "group_constants",
      std::vector<std::string>{"TOTXS",
                               "FISSXS",
                               "NSF",
                               "FISSE",
                               "RECIPVEL",
                               "CHI_T",
                               "CHI_P",
                               "CHI_D",
                               "SPN",
                               "BETA_EFF",
                               "DECAY_CONSTANT"},
      "Group constants to be determined.");
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addParam<int>("L", 2, "Maximum scattering moment");
  params.addParam<Real>("void_constant", 0.,
      "The limit under which stabilization is applied for near-void and void regions");
  params.addParam<Real>("stabilization_constant", 1.,
      "Maximum stabilization multiplier.");
  MooseEnum h_type("max min", "max");
  params.addParam<MooseEnum>("h_type", h_type,
      "Whether to use the maximum or minimum vertex separation in calculating the stabilization "
      "parameter");

  // the following two lines esentially make the two parameters optional
  params.set<std::vector<std::string>>("prop_names") = std::vector<std::string>();
  params.set<std::vector<Real>>("prop_values") = std::vector<Real>();
  return params;
}

MoltresSNMaterial::MoltresSNMaterial(const InputParameters & parameters)
  : GenericConstantMaterial(parameters),
    _temperature(coupledValue("temperature")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _totxs(declareProperty<std::vector<Real>>("totxs")),
    _fissxs(declareProperty<std::vector<Real>>("fissxs")),
    _nsf(declareProperty<std::vector<Real>>("nsf")),
    _fisse(declareProperty<std::vector<Real>>("fisse")),
    _recipvel(declareProperty<std::vector<Real>>("recipvel")),
    _chi_t(declareProperty<std::vector<Real>>("chi_t")),
    _chi_p(declareProperty<std::vector<Real>>("chi_p")),
    _chi_d(declareProperty<std::vector<Real>>("chi_d")),
    _scatter(declareProperty<std::vector<Real>>("scatter")),
    _beta_eff(declareProperty<std::vector<Real>>("beta_eff")),
    _beta(declareProperty<Real>("beta")),
    _decay_constant(declareProperty<std::vector<Real>>("decay_constant")),
    _diffcoef(declareProperty<std::vector<Real>>("diffcoef")),
    _d_totxs_d_temp(declareProperty<std::vector<Real>>("d_totxs_d_temp")),
    _d_fissxs_d_temp(declareProperty<std::vector<Real>>("d_fissxs_d_temp")),
    _d_nsf_d_temp(declareProperty<std::vector<Real>>("d_nsf_d_temp")),
    _d_fisse_d_temp(declareProperty<std::vector<Real>>("d_fisse_d_temp")),
    _d_recipvel_d_temp(declareProperty<std::vector<Real>>("d_recipvel_d_temp")),
    _d_chi_t_d_temp(declareProperty<std::vector<Real>>("d_chi_t_d_temp")),
    _d_chi_p_d_temp(declareProperty<std::vector<Real>>("d_chi_p_d_temp")),
    _d_chi_d_d_temp(declareProperty<std::vector<Real>>("d_chi_d_d_temp")),
    _d_scatter_d_temp(declareProperty<std::vector<Real>>("d_scatter_d_temp")),
    _d_beta_eff_d_temp(declareProperty<std::vector<Real>>("d_beta_eff_d_temp")),
    _d_beta_d_temp(declareProperty<Real>("d_beta_d_temp")),
    _d_decay_constant_d_temp(declareProperty<std::vector<Real>>("d_decay_constant_d_temp")),
    _d_diffcoef_d_temp(declareProperty<std::vector<Real>>("d_diffcoef_d_temp")),
    _interp_type(getParam<MooseEnum>("interp_type")),
    _group_consts(getParam<std::vector<std::string>>("group_constants")),
    _material_key(getParam<std::string>("material_key")),
    _N(getParam<unsigned int>("N")),
    _L(getParam<int>("L")),
    _sigma(getParam<Real>("void_constant")),
    _c(getParam<Real>("stabilization_constant")),
    _h_type(getParam<MooseEnum>("h_type")),
    _tau_sn(declareProperty<std::vector<Real>>("tau_sn"))
{
  auto n = _xsec_names.size();
  for (decltype(n) j = 0; j < n; ++j)
  {
    if (_xsec_names[j] == "SPN")
      _vec_lengths[_xsec_names[j]] = _num_groups * _num_groups * (_L+1);
    else if (_xsec_names[j].compare("GTRANSFXS") == 0)
      _vec_lengths[_xsec_names[j]] = _num_groups * _num_groups;
    else if (_xsec_names[j] == "BETA_EFF" || _xsec_names[j] == "DECAY_CONSTANT")
      _vec_lengths[_xsec_names[j]] = _num_precursor_groups;
    else
      _vec_lengths[_xsec_names[j]] = _num_groups;
  }

  std::string base_file = getParam<std::string>("base_file");
  const std::string & file_name_ref = base_file;
  std::ifstream myfile(file_name_ref.c_str());
  if (!myfile.good())
    mooseError("Unable to open XS file: " + base_file);
  nlohmann::json xs_root;
  myfile >> xs_root;

  int k = 0;
  auto temp_root = xs_root[_material_key]["temp"];
  _XsTemperature.resize(temp_root.size());

  for (auto & el : temp_root.items())
  {
    _XsTemperature[k] = el.value().get<int>();
    k = k + 1;
  }

  Construct(xs_root);
}

void
MoltresSNMaterial::dummyComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _totxs[_qp][i] = _xsec_map["TOTXS"][i][0];
    _fissxs[_qp][i] = _xsec_map["FISSXS"][i][0];
    _nsf[_qp][i] = _xsec_map["NSF"][i][0];
    _fisse[_qp][i] = _xsec_map["FISSE"][i][0] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _recipvel[_qp][i] = _xsec_map["RECIPVEL"][i][0];
    _chi_t[_qp][i] = _xsec_map["CHI_T"][i][0];
    _chi_p[_qp][i] = _xsec_map["CHI_P"][i][0];
    _chi_d[_qp][i] = _xsec_map["CHI_D"][i][0];
    _diffcoef[_qp][i] = _xsec_map["DIFFCOEF"][i][0];
    _d_totxs_d_temp[_qp][i] = 0;
    _d_fissxs_d_temp[_qp][i] = 0;
    _d_nsf_d_temp[_qp][i] = 0;
    _d_fisse_d_temp[_qp][i] = 0;
    _d_recipvel_d_temp[_qp][i] = 0;
    _d_chi_t_d_temp[_qp][i] = 0;
    _d_chi_p_d_temp[_qp][i] = 0;
    _d_chi_d_d_temp[_qp][i] = 0;
    _d_diffcoef_d_temp[_qp][i] = 0;
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups * (_L+1); ++i)
  {
    _scatter[_qp][i] = _xsec_map["SPN"][i][0];
    _d_scatter_d_temp[_qp][i] = 0;
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = _xsec_map["BETA_EFF"][i][0];
    _d_beta_eff_d_temp[_qp][i] = 0;
    _beta[_qp] += _beta_eff[_qp][i];
    _decay_constant[_qp][i] = _xsec_map["DECAY_CONSTANT"][i][0];
    _d_decay_constant_d_temp[_qp][i] = 0;
  }
}

void
MoltresSNMaterial::splineComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _totxs[_qp][i] = _xsec_spline_interpolators["TOTXS"][i].sample(_temperature[_qp]);
    _fissxs[_qp][i] = _xsec_spline_interpolators["FISSXS"][i].sample(_temperature[_qp]);
    _nsf[_qp][i] = _xsec_spline_interpolators["NSF"][i].sample(_temperature[_qp]);
    _fisse[_qp][i] = _xsec_spline_interpolators["FISSE"][i].sample(_temperature[_qp]) * 1e6 *
                     1.6e-19; // convert from MeV to Joules
    _recipvel[_qp][i] = _xsec_spline_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
    _chi_t[_qp][i] = _xsec_spline_interpolators["CHI_T"][i].sample(_temperature[_qp]);
    _chi_p[_qp][i] = _xsec_spline_interpolators["CHI_P"][i].sample(_temperature[_qp]);
    _chi_d[_qp][i] = _xsec_spline_interpolators["CHI_D"][i].sample(_temperature[_qp]);
    _diffcoef[_qp][i] = _xsec_spline_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
    _d_totxs_d_temp[_qp][i] =
        _xsec_spline_interpolators["TOTXS"][i].sampleDerivative(_temperature[_qp]);
    _d_fissxs_d_temp[_qp][i] =
        _xsec_spline_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]);
    _d_nsf_d_temp[_qp][i] =
        _xsec_spline_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]);
    _d_fisse_d_temp[_qp][i] =
        _xsec_spline_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]) * 1e6 *
        1.6e-19; // convert from MeV to Joules
    _d_recipvel_d_temp[_qp][i] =
        _xsec_spline_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_t_d_temp[_qp][i] =
        _xsec_spline_interpolators["CHI_T"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_p_d_temp[_qp][i] =
        _xsec_spline_interpolators["CHI_P"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_d_d_temp[_qp][i] =
        _xsec_spline_interpolators["CHI_D"][i].sampleDerivative(_temperature[_qp]);
    _d_diffcoef_d_temp[_qp][i] =
        _xsec_spline_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups * (_L+1); ++i)
  {
    _scatter[_qp][i] = _xsec_spline_interpolators["SPN"][i].sample(_temperature[_qp]);
    _d_scatter_d_temp[_qp][i] =
        _xsec_spline_interpolators["SPN"][i].sampleDerivative(_temperature[_qp]);
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
MoltresSNMaterial::monotoneCubicComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _totxs[_qp][i] = _xsec_monotone_cubic_interpolators["TOTXS"][i].sample(_temperature[_qp]);
    _fissxs[_qp][i] = _xsec_monotone_cubic_interpolators["FISSXS"][i].sample(_temperature[_qp]);
    _nsf[_qp][i] = _xsec_monotone_cubic_interpolators["NSF"][i].sample(_temperature[_qp]);
    _fisse[_qp][i] = _xsec_monotone_cubic_interpolators["FISSE"][i].sample(_temperature[_qp]) *
                     1e6 * 1.6e-19; // convert from MeV to Joules
    _recipvel[_qp][i] = _xsec_monotone_cubic_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
    _chi_t[_qp][i] = _xsec_monotone_cubic_interpolators["CHI_T"][i].sample(_temperature[_qp]);
    _chi_p[_qp][i] = _xsec_monotone_cubic_interpolators["CHI_P"][i].sample(_temperature[_qp]);
    _chi_d[_qp][i] = _xsec_monotone_cubic_interpolators["CHI_D"][i].sample(_temperature[_qp]);
    _diffcoef[_qp][i] = _xsec_monotone_cubic_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
    _d_totxs_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["TOTXS"][i].sampleDerivative(_temperature[_qp]);
    _d_fissxs_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]);
    _d_nsf_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]);
    _d_fisse_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]) * 1e6 *
        1.6e-19; // convert from MeV to Joules
    _d_recipvel_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_t_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["CHI_T"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_p_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["CHI_P"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_d_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["CHI_D"][i].sampleDerivative(_temperature[_qp]);
    _d_diffcoef_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups * (_L+1); ++i)
  {
    _scatter[_qp][i] =
        _xsec_monotone_cubic_interpolators["SPN"][i].sample(_temperature[_qp]);
    _d_scatter_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["SPN"][i].sampleDerivative(_temperature[_qp]);
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
MoltresSNMaterial::linearComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _totxs[_qp][i] = _xsec_linear_interpolators["TOTXS"][i].sample(_temperature[_qp]);
    _fissxs[_qp][i] = _xsec_linear_interpolators["FISSXS"][i].sample(_temperature[_qp]);
    _nsf[_qp][i] = _xsec_linear_interpolators["NSF"][i].sample(_temperature[_qp]);
    _fisse[_qp][i] = _xsec_linear_interpolators["FISSE"][i].sample(_temperature[_qp]) * 1e6 *
                     1.6e-19; // convert from MeV to Joules
    _recipvel[_qp][i] = _xsec_linear_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
    _chi_t[_qp][i] = _xsec_linear_interpolators["CHI_T"][i].sample(_temperature[_qp]);
    _chi_p[_qp][i] = _xsec_linear_interpolators["CHI_P"][i].sample(_temperature[_qp]);
    _chi_d[_qp][i] = _xsec_linear_interpolators["CHI_D"][i].sample(_temperature[_qp]);
    _diffcoef[_qp][i] = _xsec_linear_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
    _d_totxs_d_temp[_qp][i] =
        _xsec_linear_interpolators["TOTXS"][i].sampleDerivative(_temperature[_qp]);
    _d_fissxs_d_temp[_qp][i] =
        _xsec_linear_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]);
    _d_nsf_d_temp[_qp][i] =
        _xsec_linear_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]);
    _d_fisse_d_temp[_qp][i] =
        _xsec_linear_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]) * 1e6 *
        1.6e-19; // convert from MeV to Joules
    _d_recipvel_d_temp[_qp][i] =
        _xsec_linear_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_t_d_temp[_qp][i] =
        _xsec_linear_interpolators["CHI_T"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_p_d_temp[_qp][i] =
        _xsec_linear_interpolators["CHI_P"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_d_d_temp[_qp][i] =
        _xsec_linear_interpolators["CHI_D"][i].sampleDerivative(_temperature[_qp]);
    _d_diffcoef_d_temp[_qp][i] =
        _xsec_linear_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups * (_L+1); ++i)
  {
    _scatter[_qp][i] = _xsec_linear_interpolators["SPN"][i].sample(_temperature[_qp]);
    _d_scatter_d_temp[_qp][i] =
        _xsec_linear_interpolators["SPN"][i].sampleDerivative(_temperature[_qp]);
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
MoltresSNMaterial::preComputeQpProperties()
{
  for (unsigned int i = 0; i < _num_props; i++)
    (*_properties[i])[_qp] = _prop_values[i];
  _totxs[_qp].resize(_num_groups);
  _fissxs[_qp].resize(_num_groups);
  _nsf[_qp].resize(_num_groups);
  _fisse[_qp].resize(_num_groups);
  _recipvel[_qp].resize(_num_groups);
  _chi_t[_qp].resize(_num_groups);
  _chi_p[_qp].resize(_num_groups);
  _chi_d[_qp].resize(_num_groups);
  _scatter[_qp].resize(_num_groups * _num_groups * (_L+1));
  _beta_eff[_qp].resize(_num_precursor_groups);
  _decay_constant[_qp].resize(_num_precursor_groups);
  _diffcoef[_qp].resize(_num_groups);

  _d_totxs_d_temp[_qp].resize(_num_groups);
  _d_fissxs_d_temp[_qp].resize(_num_groups);
  _d_nsf_d_temp[_qp].resize(_num_groups);
  _d_fisse_d_temp[_qp].resize(_num_groups);
  _d_recipvel_d_temp[_qp].resize(_num_groups);
  _d_chi_t_d_temp[_qp].resize(_num_groups);
  _d_chi_p_d_temp[_qp].resize(_num_groups);
  _d_chi_d_d_temp[_qp].resize(_num_groups);
  _d_scatter_d_temp[_qp].resize(_num_groups * _num_groups * (_L+1));
  _d_beta_eff_d_temp[_qp].resize(_num_precursor_groups);
  _d_decay_constant_d_temp[_qp].resize(_num_precursor_groups);
  _d_diffcoef_d_temp[_qp].resize(_num_groups);

  _tau_sn[_qp].resize(_num_groups);
}

void
MoltresSNMaterial::Construct(nlohmann::json xs_root)
{
  std::set<std::string> gc_set(_group_consts.begin(), _group_consts.end());
  bool oneInfo = false;
  for (unsigned int j = 0; j < _xsec_names.size(); ++j)
  {
    auto o = _vec_lengths[_xsec_names[j]];
    auto L = _XsTemperature.size();

    _xsec_linear_interpolators[_xsec_names[j]].resize(o);
    _xsec_spline_interpolators[_xsec_names[j]].resize(o);
    _xsec_monotone_cubic_interpolators[_xsec_names[j]].resize(o);

    _xsec_map[_xsec_names[j]].resize(o);

    if (gc_set.find(_xsec_names[j]) != gc_set.end())
    {
      for (decltype(_XsTemperature.size()) l = 0; l < L; ++l)
      {
        auto temp_key = std::to_string(static_cast<int>(_XsTemperature[l]));
        auto dataset = xs_root[_material_key][temp_key][_xsec_names[j]];
        if (_xsec_names[j] == "CHI_D" && dataset.empty())
        {
          for (decltype(_num_groups) k = 1; k < _num_groups; ++k)
            _xsec_map["CHI_D"][k].push_back(0.0);
          _xsec_map["CHI_D"][0].push_back(1.0);
          mooseWarning(
              "CHI_D data missing -> assume delayed neutrons born in top group for material " +
              _name);
          continue;
        }
        if (dataset.empty())
          mooseError("Unable to open database " + _material_key + "/" + temp_key + "/" +
                     _xsec_names[j]);

        int dims = dataset.size();
        if (o == 0 and !oneInfo)
        {
          mooseInfo("Only precursor material data initialized (num_groups = 0) for material " + _name);
          oneInfo = true;
        }
        if ((o!=dims && o!=0 && _xsec_names[j] != "SPN") || (dims<(_L+1) && _xsec_names[j] == "SPN"))
          mooseError("The number of " + _material_key + "/" + temp_key + "/" +
                     _xsec_names[j] + " values does not match the "
                     "num_groups/num_precursor_groups parameter. " +
                     std::to_string(dims) + "!=" + std::to_string(o));
        if (_xsec_names[j] == "SPN")
          for (auto i = 0; i < (_L+1); ++i)
            for (auto k = 0; k < o/(_L+1); ++k)
              _xsec_map[_xsec_names[j]][i*o/(_L+1)+k].push_back(dataset[i][k].get<double>());
        else
          for (auto k = 0; k < o; ++k)
            _xsec_map[_xsec_names[j]][k].push_back(dataset[k].get<double>());
      }
    } else
    {
      // Initialize excluded group constants as zero values
      for (decltype(_XsTemperature.size()) l = 0; l < L; ++l)
        for (auto k = 0; k < o; ++k)
          _xsec_map[_xsec_names[j]][k].push_back(0.);
    }
    switch (_interp_type)
    {
      case NONE:
        if (L > 1)
            mooseError(
                "Group constant data for '" + _material_key + "' provided at "
                "multiple temperatures with interp_type=none. Remove extra temperature data or "
                "change interpolation scheme.");
        break;
      case LINEAR:
        if (_xsec_names[j] == "SPN")
          for (auto i = 0; i < (_L+1); ++i)
            for (auto k = 0; k < o/(_L+1); ++k)
              _xsec_linear_interpolators[_xsec_names[j]][i*o/(_L+1)+k].setData(
                _XsTemperature,
                _xsec_map[_xsec_names[j]][i*o/(_L+1)+k]);
        else
          for (auto k = 0; k < o; ++k)
            _xsec_linear_interpolators[_xsec_names[j]][k].setData(_XsTemperature,
                                                                  _xsec_map[_xsec_names[j]][k]);
        break;
      case SPLINE:
        if (_xsec_names[j] == "SPN")
          for (auto i = 0; i < (_L+1); ++i)
            for (auto k = 0; k < o/(_L+1); ++k)
              _xsec_spline_interpolators[_xsec_names[j]][i*o/(_L+1)+k].setData(
                _XsTemperature,
                _xsec_map[_xsec_names[j]][i*o/(_L+1)+k]);
        else
          for (auto k = 0; k < o; ++k)
            _xsec_spline_interpolators[_xsec_names[j]][k].setData(_XsTemperature,
                                                                  _xsec_map[_xsec_names[j]][k]);
        break;
      case MONOTONE_CUBIC:
        if (L < 3)
          mooseError("Monotone cubic interpolation requires at least three data points.");
        if (_xsec_names[j] == "SPN")
          for (auto i = 0; i < (_L+1); ++i)
            for (auto k = 0; k < o/(_L+1); ++k)
              _xsec_monotone_cubic_interpolators[_xsec_names[j]][i*o/(_L+1)+k].setData(
                _XsTemperature,
                _xsec_map[_xsec_names[j]][i*o/(_L+1)+k]);
        else
          for (auto k = 0; k < o; ++k)
            _xsec_monotone_cubic_interpolators[_xsec_names[j]][k].setData(_XsTemperature,
                                                                  _xsec_map[_xsec_names[j]][k]);
        break;
      default:
        mooseError("Invalid enum type for interp_type");
        break;
    }
  }
}

void
MoltresSNMaterial::computeQpProperties()
{
  preComputeQpProperties();

  switch (_interp_type)
  {
    case NONE:
      dummyComputeQpProperties();
      break;
    case LINEAR:
      linearComputeQpProperties();
      break;
    case SPLINE:
      splineComputeQpProperties();
      break;
    case MONOTONE_CUBIC:
      monotoneCubicComputeQpProperties();
      break;
    default:
      mooseError("Invalid enum type for interp_type");
      break;
  }

  Real h;
  if (_h_type == "max")
    h = _current_elem->hmax();
  else
    h = _current_elem->hmin();
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (_c * h * _totxs[_qp][i] > _sigma)
      _tau_sn[_qp][i] = 1. / (_c * _totxs[_qp][i]);
    else
      _tau_sn[_qp][i] = h / _sigma;
  }
}
