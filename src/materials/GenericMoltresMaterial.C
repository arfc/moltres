#include "GenericMoltresMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

template <>
InputParameters
validParams<GenericMoltresMaterial>()
{
  InputParameters params = validParams<GenericConstantMaterial>();
  params.addRequiredParam<std::string>(
      "property_tables_root",
      "The file root name containing interpolation tables for material properties.");
  params.addRequiredParam<unsigned>("num_groups",
                                    "The number of groups the energy spectrum is divided into.");
  params.addRequiredParam<unsigned>("num_precursor_groups",
                                    "The number of delayed neutron precursor groups.");
  params.addCoupledVar(
      "temperature", 937, "The temperature field for determining group constants.");
  MooseEnum interp_type("bicubic_spline spline least_squares monotone_cubic linear none");
  params.addRequiredParam<MooseEnum>(
      "interp_type", interp_type, "The type of interpolation to perform.");
  params.addParam<std::vector<Real>>("fuel_temp_points",
                                     "The fuel temperature interpolation points.");
  params.addParam<std::vector<Real>>("mod_temp_points",
                                     "The moderator temperature interpolation points.");
  params.addParam<PostprocessorName>("other_temp",
                                     0,
                                     "If doing bivariable interpolation, need to "
                                     "supply a postprocessor for the average "
                                     "temperature of the other material.");
  params.addParam<std::string>("material", "Must specify either *fuel* or *moderator*.");
  params.addParam<bool>(
      "sss2_input", true, "Whether serpent 2 was used to generate the input files.");
  params.addParam<PostprocessorName>(
      "peak_power_density", 0, "The postprocessor which holds the peak power density.");
  params.addParam<Real>(
      "peak_power_density_set_point", 10, "The peak power density set point in W/cm^3");
  params.addParam<Real>("controller_gain",
                        1e-2,
                        "For every W/cm^3 that the peak power density is "
                        "greater than the peak power density set point, "
                        "the absorption cross section gets incremented by "
                        "this amount");
  return params;
}

GenericMoltresMaterial::GenericMoltresMaterial(const InputParameters & parameters)
  : GenericConstantMaterial(parameters),
    _temperature(coupledValue("temperature")),
    _remxs(declareProperty<std::vector<Real>>("remxs")),
    _fissxs(declareProperty<std::vector<Real>>("fissxs")),
    _nsf(declareProperty<std::vector<Real>>("nsf")),
    _fisse(declareProperty<std::vector<Real>>("fisse")),
    _diffcoef(declareProperty<std::vector<Real>>("diffcoef")),
    _recipvel(declareProperty<std::vector<Real>>("recipvel")),
    _chi(declareProperty<std::vector<Real>>("chi")),
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
    _d_chi_d_temp(declareProperty<std::vector<Real>>("d_chi_d_temp")),
    _d_gtransfxs_d_temp(declareProperty<std::vector<Real>>("d_gtransfxs_d_temp")),
    _d_beta_eff_d_temp(declareProperty<std::vector<Real>>("d_beta_eff_d_temp")),
    _d_beta_d_temp(declareProperty<Real>("d_beta_d_temp")),
    _d_decay_constant_d_temp(declareProperty<std::vector<Real>>("d_decay_constant_d_temp")),
    _interp_type(getParam<MooseEnum>("interp_type")),
    _other_temp(getPostprocessorValue("other_temp")),
    _peak_power_density(getPostprocessorValue("peak_power_density")),
    _peak_power_density_set_point(getParam<Real>("peak_power_density_set_point")),
    _controller_gain(getParam<Real>("controller_gain"))
{
  _num_groups = getParam<unsigned>("num_groups");
  _num_precursor_groups = getParam<unsigned>("num_precursor_groups");
  std::string property_tables_root = getParam<std::string>("property_tables_root");
  std::vector<std::string> xsec_names{"FLUX",
                                      "REMXS",
                                      "FISSXS",
                                      "NUBAR",
                                      "NSF",
                                      "FISSE",
                                      "DIFFCOEF",
                                      "RECIPVEL",
                                      "CHI",
                                      "GTRANSFXS",
                                      "BETA_EFF",
                                      "DECAY_CONSTANT"};

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
  _file_map["NUBAR"] = "NUBAR";
  _file_map["DIFFCOEF"] = "DIFFCOEF";
  _file_map["BETA_EFF"] = "BETA_EFF";
  if (getParam<bool>("sss2_input"))
  {
    _file_map["FLUX"] = "FLX";
    _file_map["FISSXS"] = "FISS";
    _file_map["FISSE"] = "KAPPA";
    _file_map["RECIPVEL"] = "INVV";
    _file_map["CHI"] = "CHIT";
    _file_map["GTRANSFXS"] = "SP0";
    _file_map["DECAY_CONSTANT"] = "LAMBDA";
  }
  else
  {
    _file_map["FLUX"] = "FLUX";
    _file_map["FISSXS"] = "FISSXS";
    _file_map["FISSE"] = "FISSE";
    _file_map["RECIPVEL"] = "RECIPVEL";
    _file_map["CHI"] = "CHI";
    _file_map["GTRANSFXS"] = "GTRANSFXS";
    _file_map["DECAY_CONSTANT"] = "DECAY_CONSTANT";
  }

  if (_interp_type == "least_squares")
    leastSquaresConstruct(property_tables_root, xsec_names);

  else if (_interp_type == "spline")
    splineConstruct(property_tables_root, xsec_names);

  else if (_interp_type == "monotone_cubic")
    monotoneCubicConstruct(property_tables_root, xsec_names);

  else if (_interp_type == "bicubic_spline")
    bicubicSplineConstruct(property_tables_root, xsec_names, parameters);

  else if (_interp_type == "linear")
    linearConstruct(property_tables_root, xsec_names);

  else if (_interp_type == "none")
    dummyConstruct(property_tables_root, xsec_names);

  else
    mooseError("Wrong enum type");
}

void
GenericMoltresMaterial::dummyConstruct(std::string & property_tables_root,
                                       std::vector<std::string> xsec_names)
{
  Real value;
  for (decltype(xsec_names.size()) j = 0; j < xsec_names.size(); ++j)
  {
    std::vector<Real> temperature;
    std::string file_name = property_tables_root + _file_map[xsec_names[j]] + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());

    auto o = _vec_lengths[xsec_names[j]];

    _xsec_map[xsec_names[j]].resize(o);
    _xsec_spline_interpolators[xsec_names[j]].resize(o);
    if (myfile.is_open())
    {
      myfile >> value;
      temperature.push_back(value);
      for (decltype(o) k = 0; k < o; ++k)
      {
        myfile >> value;
        _xsec_map[xsec_names[j]][k] = value;
      }
    }
    else
      mooseError("Unable to open file " + file_name);
  }
}

void
GenericMoltresMaterial::splineConstruct(std::string & property_tables_root,
                                        std::vector<std::string> xsec_names)
{
  Real value;
  for (decltype(xsec_names.size()) j = 0; j < xsec_names.size(); ++j)
  {
    std::vector<Real> temperature;
    std::string file_name = property_tables_root + _file_map[xsec_names[j]] + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());

    auto o = _vec_lengths[xsec_names[j]];

    std::map<std::string, std::vector<std::vector<Real>>> xsec_map;
    xsec_map[xsec_names[j]].resize(o);
    _xsec_spline_interpolators[xsec_names[j]].resize(o);
    if (myfile.is_open())
    {
      while (myfile >> value)
      {
        temperature.push_back(value);
        for (decltype(o) k = 0; k < o; ++k)
        {
          myfile >> value;
          xsec_map[xsec_names[j]][k].push_back(value);
        }
      }
      myfile.close();
      for (decltype(o) k = 0; k < o; ++k)
        _xsec_spline_interpolators[xsec_names[j]][k].setData(temperature,
                                                             xsec_map[xsec_names[j]][k]);
    }
    else
      mooseError("Unable to open file " + file_name);
  }
}

void
GenericMoltresMaterial::monotoneCubicConstruct(std::string & property_tables_root,
                                               std::vector<std::string> xsec_names)
{
  Real value;
  for (decltype(xsec_names.size()) j = 0; j < xsec_names.size(); ++j)
  {
    std::vector<Real> temperature;
    std::string file_name = property_tables_root + _file_map[xsec_names[j]] + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());

    auto o = _vec_lengths[xsec_names[j]];

    std::map<std::string, std::vector<std::vector<Real>>> xsec_map;
    xsec_map[xsec_names[j]].resize(o);
    _xsec_monotone_cubic_interpolators[xsec_names[j]].resize(o);
    if (myfile.is_open())
    {
      while (myfile >> value)
      {
        temperature.push_back(value);
        for (decltype(o) k = 0; k < o; ++k)
        {
          myfile >> value;
          xsec_map[xsec_names[j]][k].push_back(value);
        }
      }
      myfile.close();
      for (decltype(o) k = 0; k < o; ++k)
        _xsec_monotone_cubic_interpolators[xsec_names[j]][k].setData(temperature,
                                                                     xsec_map[xsec_names[j]][k]);
    }
    else
      mooseError("Unable to open file " + file_name);
  }
}

void
GenericMoltresMaterial::linearConstruct(std::string & property_tables_root,
                                        std::vector<std::string> xsec_names)
{
  Real value;
  for (decltype(xsec_names.size()) j = 0; j < xsec_names.size(); ++j)
  {
    std::vector<Real> temperature;
    std::string file_name = property_tables_root + _file_map[xsec_names[j]] + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());

    auto o = _vec_lengths[xsec_names[j]];

    std::map<std::string, std::vector<std::vector<Real>>> xsec_map;
    xsec_map[xsec_names[j]].resize(o);
    _xsec_linear_interpolators[xsec_names[j]].resize(o);
    if (myfile.is_open())
    {
      while (myfile >> value)
      {
        temperature.push_back(value);
        for (decltype(o) k = 0; k < o; ++k)
        {
          myfile >> value;
          xsec_map[xsec_names[j]][k].push_back(value);
        }
      }
      myfile.close();
      for (decltype(o) k = 0; k < o; ++k)
        _xsec_linear_interpolators[xsec_names[j]][k].setData(temperature,
                                                             xsec_map[xsec_names[j]][k]);
    }
    else
      mooseError("Unable to open file " + file_name);
  }
}

void
GenericMoltresMaterial::bicubicSplineConstruct(std::string & property_tables_root,
                                               std::vector<std::string> xsec_names,
                                               const InputParameters & parameters)
{
  Real value;
  std::vector<Real> fuel_temperature;
  std::vector<Real> mod_temperature;
  if (isParamValid("fuel_temp_points") &&
      !(getParam<std::vector<Real>>("fuel_temp_points").empty()))
    fuel_temperature = getParam<std::vector<Real>>("fuel_temp_points");
  else
    mooseError("You forgot to supply *fuel_temp_points* for interpolation!");
  if (isParamValid("mod_temp_points") && !(getParam<std::vector<Real>>("mod_temp_points").empty()))
    mod_temperature = getParam<std::vector<Real>>("mod_temp_points");
  else
    mooseError("You forgot to supply *mod_temp_points* for interpolation!");
  auto l = fuel_temperature.size();
  auto m = mod_temperature.size();

  for (decltype(xsec_names.size()) j = 0; j < xsec_names.size(); ++j)
  {
    std::string file_name = property_tables_root + _file_map[xsec_names[j]] + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());

    auto o = _vec_lengths[xsec_names[j]];

    std::map<std::string, std::vector<std::vector<std::vector<Real>>>> xsec_map;
    xsec_map[xsec_names[j]].resize(o);
    for (decltype(o) k = 0; k < o; ++k)
      xsec_map[xsec_names[j]][k].resize(l);
    _xsec_bicubic_spline_interpolators[xsec_names[j]].resize(o);
    if (myfile.is_open())
    {
      for (decltype(l) h = 0; h < l; ++h)
      {
        for (decltype(m) i = 0; i < m; ++i)
        {
          myfile >> value;
          myfile >> value;
          for (decltype(o) k = 0; k < o; ++k)
          {
            myfile >> value;
            xsec_map[xsec_names[j]][k][h].push_back(value);
          }
        }
      }
      myfile.close();
      for (decltype(o) k = 0; k < o; ++k)
        _xsec_bicubic_spline_interpolators[xsec_names[j]][k].setData(
            fuel_temperature, mod_temperature, xsec_map[xsec_names[j]][k]);
    }
    else
      mooseError("Unable to open file " + file_name);
  }

  if (!parameters.isParamSetByUser("other_temp"))
    mooseError("If doing bivariable interpolation, you must supply a postprocessor value for the "
               "average temperature of the other material.");

  if (!isParamValid("material"))
    mooseError("User must specify whether we are in the *fuel* or *moderator*.");
  else
  {
    _material = getParam<std::string>("material");
    if (_material.compare("fuel") != 0 && _material.compare("moderator") != 0)
      mooseError("Only valid choices for material parameter are *fuel* and *moderator*.");
  }
}

void
GenericMoltresMaterial::leastSquaresConstruct(std::string & property_tables_root,
                                              std::vector<std::string> xsec_names)
{
  Real value;
  std::map<std::string, std::vector<std::vector<Real>>> xsec_map;

  // loop over type of constant, e.g. remxs, diffcoeff, etc.
  for (decltype(xsec_names.size()) i = 0; i < xsec_names.size(); ++i)
  {
    xsec_map[xsec_names[i]].resize(2);
    // loop over number of constants in least squares fit (2 for linear)
    for (unsigned int j = 0; j <= 1; ++j)
      xsec_map[xsec_names[i]][j].resize(_vec_lengths[xsec_names[i]]);
  }

  std::string file_name = property_tables_root;
  const std::string & file_name_ref = file_name;
  std::ifstream myfile(file_name_ref.c_str());

  auto m = xsec_names.size();
  if (myfile.is_open())
  {
    // loop over type of constant, e.g. remxs, diffcoeff, etc.
    for (decltype(m) i = 0; i < m; ++i)
    {
      // loop over number of groups / number of precursor groups (or number of groups squared for
      // GTRANSFXS
      auto n = _vec_lengths[xsec_names[i]];
      for (decltype(n) j = 0; j < n; ++j)
      {
        // loop over number of constants in least squares fit (2 for linear)
        for (unsigned int k = 0; k <= 1; ++k)
        {
          myfile >> value;
          xsec_map[xsec_names[i]][k][j] = value;
        }
      }
    }
  }

  _flux_consts = xsec_map["FLUX"];
  _remxs_consts = xsec_map["REMXS"];
  _fissxs_consts = xsec_map["FISSXS"];
  _nubar_consts = xsec_map["NUBAR"];
  _nsf_consts = xsec_map["NSF"];
  _fisse_consts = xsec_map["FISSE"];
  _diffcoeff_consts = xsec_map["DIFFCOEF"];
  _recipvel_consts = xsec_map["RECIPVEL"];
  _chi_consts = xsec_map["CHI"];
  _gtransfxs_consts = xsec_map["GTRANSFXS"];
  _beta_eff_consts = xsec_map["BETA_EFF"];
  _decay_constants_consts = xsec_map["DECAY_CONSTANT"];
}

void
GenericMoltresMaterial::dummyComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _xsec_map["REMXS"][i];
    _fissxs[_qp][i] = _xsec_map["FISSXS"][i];
    _nsf[_qp][i] = _xsec_map["NSF"][i];
    _fisse[_qp][i] = _xsec_map["FISSE"][i] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = _xsec_map["DIFFCOEF"][i];
    _recipvel[_qp][i] = _xsec_map["RECIPVEL"][i];
    _chi[_qp][i] = _xsec_map["CHI"][i];
    _d_remxs_d_temp[_qp][i] = _xsec_map["REMXS"][i];
    _d_fissxs_d_temp[_qp][i] = _xsec_map["FISSXS"][i];
    _d_nsf_d_temp[_qp][i] = _xsec_map["NSF"][i];
    _d_fisse_d_temp[_qp][i] = _xsec_map["FISSE"][i] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = _xsec_map["DIFFCOEF"][i];
    _d_recipvel_d_temp[_qp][i] = _xsec_map["RECIPVEL"][i];
    _d_chi_d_temp[_qp][i] = _xsec_map["CHI"][i];
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] = _xsec_map["GTRANSFXS"][i];
    _d_gtransfxs_d_temp[_qp][i] = _xsec_map["GTRANSFXS"][i];
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = _xsec_map["BETA_EFF"][i];
    _d_beta_eff_d_temp[_qp][i] = _xsec_map["BETA_EFF"][i];
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] = _xsec_map["DECAY_CONSTANT"][i];
    _d_decay_constant_d_temp[_qp][i] = _xsec_map["DECAY_CONSTANT"][i];
  }
}

void
GenericMoltresMaterial::splineComputeQpProperties()
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
    _chi[_qp][i] = _xsec_spline_interpolators["CHI"][i].sample(_temperature[_qp]);
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
    _d_chi_d_temp[_qp][i] =
        _xsec_spline_interpolators["CHI"][i].sampleDerivative(_temperature[_qp]);
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
GenericMoltresMaterial::monotoneCubicComputeQpProperties()
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
    _chi[_qp][i] = _xsec_monotone_cubic_interpolators["CHI"][i].sample(_temperature[_qp]);
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
    _d_chi_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["CHI"][i].sampleDerivative(_temperature[_qp]);
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
GenericMoltresMaterial::linearComputeQpProperties()
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
    _chi[_qp][i] = _xsec_linear_interpolators["CHI"][i].sample(_temperature[_qp]);
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
    _d_chi_d_temp[_qp][i] =
        _xsec_linear_interpolators["CHI"][i].sampleDerivative(_temperature[_qp]);
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
GenericMoltresMaterial::fuelBicubic()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] =
        _xsec_bicubic_spline_interpolators["REMXS"][i].sample(_temperature[_qp], _other_temp);
    _fissxs[_qp][i] =
        _xsec_bicubic_spline_interpolators["FISSXS"][i].sample(_temperature[_qp], _other_temp);
    _nsf[_qp][i] =
        _xsec_bicubic_spline_interpolators["NSF"][i].sample(_temperature[_qp], _other_temp);
    _fisse[_qp][i] =
        _xsec_bicubic_spline_interpolators["FISSE"][i].sample(_temperature[_qp], _other_temp) *
        1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] =
        _xsec_bicubic_spline_interpolators["DIFFCOEF"][i].sample(_temperature[_qp], _other_temp);
    _recipvel[_qp][i] =
        _xsec_bicubic_spline_interpolators["RECIPVEL"][i].sample(_temperature[_qp], _other_temp);
    _chi[_qp][i] =
        _xsec_bicubic_spline_interpolators["CHI"][i].sample(_temperature[_qp], _other_temp);
    _d_remxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["REMXS"][i].sampleDerivative(
        _temperature[_qp], _other_temp, 1);
    _d_fissxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["FISSXS"][i].sampleDerivative(
        _temperature[_qp], _other_temp, 1);
    _d_nsf_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["NSF"][i].sampleDerivative(
        _temperature[_qp], _other_temp, 1);
    _d_fisse_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["FISSE"][i].sampleDerivative(
                                  _temperature[_qp], _other_temp, 1) *
                              1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["DIFFCOEF"][i].sampleDerivative(
        _temperature[_qp], _other_temp, 1);
    _d_recipvel_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["RECIPVEL"][i].sampleDerivative(
        _temperature[_qp], _other_temp, 1);
    _d_chi_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI"][i].sampleDerivative(
        _temperature[_qp], _other_temp, 1);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] =
        _xsec_bicubic_spline_interpolators["GTRANSFXS"][i].sample(_temperature[_qp], _other_temp);
    _d_gtransfxs_d_temp[_qp][i] =
        _xsec_bicubic_spline_interpolators["GTRANSFXS"][i].sampleDerivative(
            _temperature[_qp], _other_temp, 1);
  }
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] =
        _xsec_bicubic_spline_interpolators["BETA_EFF"][i].sample(_temperature[_qp], _other_temp);
    _d_beta_eff_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["BETA_EFF"][i].sampleDerivative(
        _temperature[_qp], _other_temp, 1);
    _decay_constant[_qp][i] = _xsec_bicubic_spline_interpolators["DECAY_CONSTANT"][i].sample(
        _temperature[_qp], _other_temp);
    _d_decay_constant_d_temp[_qp][i] =
        _xsec_bicubic_spline_interpolators["DECAY_CONSTANT"][i].sampleDerivative(
            _temperature[_qp], _other_temp, 1);
  }
}

void
GenericMoltresMaterial::moderatorBicubic()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] =
        _xsec_bicubic_spline_interpolators["REMXS"][i].sample(_other_temp, _temperature[_qp]);
    _fissxs[_qp][i] =
        _xsec_bicubic_spline_interpolators["FISSXS"][i].sample(_other_temp, _temperature[_qp]);
    _nsf[_qp][i] =
        _xsec_bicubic_spline_interpolators["NSF"][i].sample(_other_temp, _temperature[_qp]);
    _fisse[_qp][i] =
        _xsec_bicubic_spline_interpolators["FISSE"][i].sample(_other_temp, _temperature[_qp]) *
        1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] =
        _xsec_bicubic_spline_interpolators["DIFFCOEF"][i].sample(_other_temp, _temperature[_qp]);
    _recipvel[_qp][i] =
        _xsec_bicubic_spline_interpolators["RECIPVEL"][i].sample(_other_temp, _temperature[_qp]);
    _chi[_qp][i] =
        _xsec_bicubic_spline_interpolators["CHI"][i].sample(_other_temp, _temperature[_qp]);
    _d_remxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["REMXS"][i].sampleDerivative(
        _other_temp, _temperature[_qp], 2);
    _d_fissxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["FISSXS"][i].sampleDerivative(
        _other_temp, _temperature[_qp], 2);
    _d_nsf_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["NSF"][i].sampleDerivative(
        _other_temp, _temperature[_qp], 2);
    _d_fisse_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["FISSE"][i].sampleDerivative(
                                  _other_temp, _temperature[_qp], 2) *
                              1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["DIFFCOEF"][i].sampleDerivative(
        _other_temp, _temperature[_qp], 2);
    _d_recipvel_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["RECIPVEL"][i].sampleDerivative(
        _other_temp, _temperature[_qp], 2);
    _d_chi_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI"][i].sampleDerivative(
        _other_temp, _temperature[_qp], 2);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] =
        _xsec_bicubic_spline_interpolators["GTRANSFXS"][i].sample(_other_temp, _temperature[_qp]);
    _d_gtransfxs_d_temp[_qp][i] =
        _xsec_bicubic_spline_interpolators["GTRANSFXS"][i].sampleDerivative(
            _other_temp, _temperature[_qp], 2);
  }
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] =
        _xsec_bicubic_spline_interpolators["BETA_EFF"][i].sample(_other_temp, _temperature[_qp]);
    _d_beta_eff_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["BETA_EFF"][i].sampleDerivative(
        _other_temp, _temperature[_qp], 2);
    _decay_constant[_qp][i] = _xsec_bicubic_spline_interpolators["DECAY_CONSTANT"][i].sample(
        _other_temp, _temperature[_qp]);
    _d_decay_constant_d_temp[_qp][i] =
        _xsec_bicubic_spline_interpolators["DECAY_CONSTANT"][i].sampleDerivative(
            _other_temp, _temperature[_qp], 2);
  }
}

void
GenericMoltresMaterial::bicubicSplineComputeQpProperties()
{
  if (_material.compare("fuel") == 0)
    fuelBicubic();

  else if (_material.compare("moderator") == 0)
    moderatorBicubic();

  else
    mooseError("Only valid choices for material parameter are *fuel* and *moderator*.");
}

void
GenericMoltresMaterial::leastSquaresComputeQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _remxs_consts[0][i] * _temperature[_qp] + _remxs_consts[1][i];
    _fissxs[_qp][i] = _fissxs_consts[0][i] * _temperature[_qp] + _fissxs_consts[1][i];
    _nsf[_qp][i] = _nsf_consts[0][i] * _temperature[_qp] + _nsf_consts[1][i];
    _fisse[_qp][i] = (_fisse_consts[0][i] * _temperature[_qp] + _fisse_consts[1][i]) * 1e6 *
                     1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = _diffcoeff_consts[0][i] * _temperature[_qp] + _diffcoeff_consts[1][i];
    _recipvel[_qp][i] = _recipvel_consts[0][i] * _temperature[_qp] + _recipvel_consts[1][i];
    _chi[_qp][i] = _chi_consts[0][i] * _temperature[_qp] + _chi_consts[1][i];
    _d_remxs_d_temp[_qp][i] = _remxs_consts[0][i];
    _d_fissxs_d_temp[_qp][i] = _fissxs_consts[0][i];
    _d_nsf_d_temp[_qp][i] = _nsf_consts[0][i];
    _d_fisse_d_temp[_qp][i] = _fisse_consts[0][i] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = _diffcoeff_consts[0][i];
    _d_recipvel_d_temp[_qp][i] = _recipvel_consts[0][i];
    _d_chi_d_temp[_qp][i] = _chi_consts[0][i];
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] = _gtransfxs_consts[0][i] * _temperature[_qp] + _gtransfxs_consts[1][i];
    _d_gtransfxs_d_temp[_qp][i] = _gtransfxs_consts[0][i];
  }
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = _beta_eff_consts[0][i] * _temperature[_qp] + _beta_eff_consts[1][i];
    _d_beta_eff_d_temp[_qp][i] = _beta_eff_consts[0][i];
    _decay_constant[_qp][i] =
        _decay_constants_consts[0][i] * _temperature[_qp] + _decay_constants_consts[1][i];
    _d_decay_constant_d_temp[_qp][i] = _decay_constants_consts[0][i];
  }
}

void
GenericMoltresMaterial::computeQpProperties()
{
  for (unsigned int i = 0; i < _num_props; i++)
    (*_properties[i])[_qp] = _prop_values[i];

  _remxs[_qp].resize(_vec_lengths["REMXS"]);
  _fissxs[_qp].resize(_vec_lengths["FISSXS"]);
  _nsf[_qp].resize(_vec_lengths["NSF"]);
  _fisse[_qp].resize(_vec_lengths["FISSE"]);
  _diffcoef[_qp].resize(_vec_lengths["DIFFCOEF"]);
  _recipvel[_qp].resize(_vec_lengths["RECIPVEL"]);
  _chi[_qp].resize(_vec_lengths["CHI"]);
  _gtransfxs[_qp].resize(_vec_lengths["GTRANSFXS"]);
  _beta_eff[_qp].resize(_vec_lengths["BETA_EFF"]);
  _decay_constant[_qp].resize(_vec_lengths["DECAY_CONSTANT"]);
  _d_remxs_d_temp[_qp].resize(_vec_lengths["REMXS"]);
  _d_fissxs_d_temp[_qp].resize(_vec_lengths["FISSXS"]);
  _d_nsf_d_temp[_qp].resize(_vec_lengths["NSF"]);
  _d_fisse_d_temp[_qp].resize(_vec_lengths["FISSE"]);
  _d_diffcoef_d_temp[_qp].resize(_vec_lengths["DIFFCOEF"]);
  _d_recipvel_d_temp[_qp].resize(_vec_lengths["RECIPVEL"]);
  _d_chi_d_temp[_qp].resize(_vec_lengths["CHI"]);
  _d_gtransfxs_d_temp[_qp].resize(_vec_lengths["GTRANSFXS"]);
  _d_beta_eff_d_temp[_qp].resize(_vec_lengths["BETA_EFF"]);
  _d_decay_constant_d_temp[_qp].resize(_vec_lengths["DECAY_CONSTANT"]);

  if (_interp_type == "spline")
    splineComputeQpProperties();

  else if (_interp_type == "monotone_cubic")
    monotoneCubicComputeQpProperties();

  else if (_interp_type == "linear")
    linearComputeQpProperties();

  else if (_interp_type == "bicubic_spline")
    bicubicSplineComputeQpProperties();

  else if (_interp_type == "least_squares")
    leastSquaresComputeQpProperties();

  else if (_interp_type == "none")
    dummyComputeQpProperties();

  for (unsigned i = 0; i < _num_groups; ++i)
    _remxs[_qp][i] += _controller_gain * (_peak_power_density - _peak_power_density_set_point);
}
