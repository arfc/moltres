#include "GenericMoltresMaterial.h"
#include "MooseUtils.h"

// #define PRINT(var) #var

registerMooseObject("MoltresApp", GenericMoltresMaterial);

template <>
InputParameters
validParams<GenericMoltresMaterial>()
{
  InputParameters params = validParams<NuclearMaterial>();
  params.addRequiredParam<std::string>(
      "property_tables_root",
      "The file root name containing interpolation tables for material properties.");
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

  // the following two lines esentially make the two parameters optional
  params.set<std::vector<std::string>>("prop_names") = std::vector<std::string>();
  params.set<std::vector<Real>>("prop_values") = std::vector<Real>();
  return params;
}

GenericMoltresMaterial::GenericMoltresMaterial(const InputParameters & parameters)
  : NuclearMaterial(parameters),
    _other_temp(getPostprocessorValue("other_temp")),
    _peak_power_density(getPostprocessorValue("peak_power_density")),
    _peak_power_density_set_point(getParam<Real>("peak_power_density_set_point")),
    _controller_gain(getParam<Real>("controller_gain"))
{
  if (parameters.isParamSetByUser("peak_power_density"))
    _perform_control = true;
  else
    _perform_control = false;

  _num_groups = getParam<unsigned>("num_groups");
  _num_precursor_groups = getParam<unsigned>("num_precursor_groups");
  std::string property_tables_root = getParam<std::string>("property_tables_root");
  std::vector<std::string> xsec_names{"REMXS",
                                      "FISSXS",
                                      "NSF",
                                      "FISSE",
                                      "DIFFCOEF",
                                      "RECIPVEL",
                                      "CHI",
                                      "CHI_D",
                                      "GTRANSFXS",
                                      "BETA_EFF",
                                      "DECAY_CONSTANT"};


  _file_map["REMXS"] = "REMXS";
  _file_map["NSF"] = "NSF";
  _file_map["DIFFCOEF"] = "DIFFCOEF";
  _file_map["BETA_EFF"] = "BETA_EFF";
  if (getParam<bool>("sss2_input"))
  {
    _file_map["FISSXS"] = "FISS";
    _file_map["FISSE"] = "KAPPA";
    _file_map["RECIPVEL"] = "INVV";

    //chi_t backwards compatibility on older input files:
    std::string file_name = property_tables_root + "CHIP" + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());
    if (myfile.good())
    {
      _file_map["CHI"] = "CHIP";
    }
    else
    {
      _file_map["CHI"] = "CHIT";
      mooseWarning("CHI_P data missing -> using CHI_T data instead for material"
		      + _name
		      + ". Extract XS data using latest script for CHI_P.");
    }

    _file_map["CHI_D"] = "CHID";
    _file_map["GTRANSFXS"] = "SP0";
    _file_map["DECAY_CONSTANT"] = "LAMBDA";
  }
  else
  {
    _file_map["FISSXS"] = "FISSXS";
    _file_map["FISSE"] = "FISSE";
    _file_map["RECIPVEL"] = "RECIPVEL";
    _file_map["CHI"] = "CHI";
    _file_map["CHI_D"] = "CHI_D";
    _file_map["GTRANSFXS"] = "GTRANSFXS";
    _file_map["DECAY_CONSTANT"] = "DECAY_CONSTANT";
  }
  switch (_interp_type)
  {
    case LSQ:
      leastSquaresConstruct(property_tables_root, xsec_names);
      break;
    case SPLINE:
      splineConstruct(property_tables_root, xsec_names);
      break;
    case MONOTONE_CUBIC:
      monotoneCubicConstruct(property_tables_root, xsec_names);
      break;
    case BICUBIC:
      bicubicSplineConstruct(property_tables_root, xsec_names, parameters);
      break;
    case LINEAR:
      linearConstruct(property_tables_root, xsec_names);
      break;
    case NONE:
      dummyConstruct(property_tables_root, xsec_names);
      break;
    default:
      mooseError("Invalid enum type for interp_type");
      break;
  }
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

    //chi_d backwards compatibility on unit tests:
    if (xsec_names[j]=="CHI_D" and not myfile.good())
    {
      for (decltype(o) k = 0; k < o; ++k)
        if (o != 0)
          _xsec_map["CHI_D"][k] = 0.0;

      _xsec_map["CHI_D"][0] = 1.0;
      mooseWarning("CHI_D data missing -> assume delayed neutrons born in top group for material" + _name);
      continue;
    }

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
  int tempLength = 0;
  bool onewarn = false;
  std::vector<Real> oldtemperature;
  for (decltype(xsec_names.size()) j = 0; j < xsec_names.size(); ++j)
  {
    std::vector<Real> temperature;
    std::string file_name = property_tables_root + _file_map[xsec_names[j]] + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());
    std::map<std::string, std::vector<std::vector<Real>>> xsec_map;
    auto o = _vec_lengths[xsec_names[j]];
    xsec_map[xsec_names[j]].resize(o);
    _xsec_spline_interpolators[xsec_names[j]].resize(o);

    //chi_d backwards compatibility on unit tests:
    if (xsec_names[j]=="CHI_D" and not myfile.good())
    {
      for (decltype(o) k = 0; k < o; ++k)
        for (int i = 0; i < tempLength; ++i)
          xsec_map["CHI_D"][k].push_back(0.0);

      for (int i = 0; i < tempLength; ++i)
        // o!=0 avoids segfault for prec-only problems (diracHX test)
        if (o != 0)
          xsec_map["CHI_D"][0][i] = 1.0;

      if (!onewarn)
      {
        mooseWarning("CHI_D data missing -> assume delayed neutrons born in top group for material" + _name);
        onewarn = true;
      }
      for (decltype(o) k = 0; k < o; ++k)
        _xsec_spline_interpolators[xsec_names[j]][k].setData(oldtemperature,
                                                             xsec_map[xsec_names[j]][k]);
      continue;
    }


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
      tempLength = temperature.size();
      myfile.close();
      for (decltype(o) k = 0; k < o; ++k)
      {
        oldtemperature = temperature;
        _xsec_spline_interpolators[xsec_names[j]][k].setData(temperature,
                                                             xsec_map[xsec_names[j]][k]);
      }
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
  int tempLength = 0;
  bool onewarn = false;
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

    //chi_d backwards compatibility on unit tests:
    if (xsec_names[j]=="CHI_D" and not myfile.good())
    {
      for (decltype(o) k = 0; k < o; ++k)
        for (int i = 0; i < tempLength; ++i)
          xsec_map["CHI_D"][k].push_back(0.0);

      for (int i = 0; i < tempLength; ++i)
        if (o != 0)
          xsec_map["CHI_D"][0][i] = 1.0;
      if (!onewarn)
      {
        mooseWarning("CHI_D data missing -> assume delayed neutrons born in top group for material" + _name);
        onewarn = true;
      }
      continue;
    }

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
      tempLength = temperature.size();
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
  int tempLength = 0;
  bool onewarn = false;
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

    //chi_d backwards compatibility on unit tests:
    if (xsec_names[j]=="CHI_D" and not myfile.good())
    {
      for (decltype(o) k = 0; k < o; ++k)
        for (int i = 0; i < tempLength; ++i)
          xsec_map["CHI_D"][k].push_back(0.0);

      for (int i = 0; i < tempLength; ++i)
        if (o != 0)
          xsec_map["CHI_D"][0][i] = 1.0;

      if (!onewarn)
      {
        mooseWarning("CHI_D data missing -> assume delayed neutrons born in top group for material" + _name);
        onewarn = true;
      }
      continue;
    }

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
      tempLength = temperature.size();
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

  _remxs_consts = xsec_map["REMXS"];
  _fissxs_consts = xsec_map["FISSXS"];
  _nsf_consts = xsec_map["NSF"];
  _fisse_consts = xsec_map["FISSE"];
  _diffcoeff_consts = xsec_map["DIFFCOEF"];
  _recipvel_consts = xsec_map["RECIPVEL"];
  _chi_consts = xsec_map["CHI"];
  _chi_d_consts = xsec_map["CHI_D"];
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
    _chi_d[_qp][i] = _xsec_map["CHI_D"][i];
    _d_remxs_d_temp[_qp][i] = _xsec_map["REMXS"][i];
    _d_fissxs_d_temp[_qp][i] = _xsec_map["FISSXS"][i];
    _d_nsf_d_temp[_qp][i] = _xsec_map["NSF"][i];
    _d_fisse_d_temp[_qp][i] = _xsec_map["FISSE"][i] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = _xsec_map["DIFFCOEF"][i];
    _d_recipvel_d_temp[_qp][i] = _xsec_map["RECIPVEL"][i];
    _d_chi_d_temp[_qp][i] = _xsec_map["CHI"][i];
    _d_chi_d_d_temp[_qp][i] = _xsec_map["CHI_D"][i];
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
    _chi_d[_qp][i] =
        _xsec_bicubic_spline_interpolators["CHI_D"][i].sample(_temperature[_qp], _other_temp);
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
    _d_chi_d_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI_D"][i].sampleDerivative(
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
    _chi_d[_qp][i] =
        _xsec_bicubic_spline_interpolators["CHI_D"][i].sample(_other_temp, _temperature[_qp]);
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
    _d_chi_d_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI_D"][i].sampleDerivative(
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
    _chi_d[_qp][i] = _chi_d_consts[0][i] * _temperature[_qp] + _chi_d_consts[1][i];
    _d_remxs_d_temp[_qp][i] = _remxs_consts[0][i];
    _d_fissxs_d_temp[_qp][i] = _fissxs_consts[0][i];
    _d_nsf_d_temp[_qp][i] = _nsf_consts[0][i];
    _d_fisse_d_temp[_qp][i] = _fisse_consts[0][i] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = _diffcoeff_consts[0][i];
    _d_recipvel_d_temp[_qp][i] = _recipvel_consts[0][i];
    _d_chi_d_temp[_qp][i] = _chi_consts[0][i];
    _d_chi_d_d_temp[_qp][i] = _chi_d_consts[0][i];
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
  _chi_d[_qp].resize(_vec_lengths["CHI_D"]);
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
  _d_chi_d_d_temp[_qp].resize(_vec_lengths["CHI_D"]);
  _d_gtransfxs_d_temp[_qp].resize(_vec_lengths["GTRANSFXS"]);
  _d_beta_eff_d_temp[_qp].resize(_vec_lengths["BETA_EFF"]);
  _d_decay_constant_d_temp[_qp].resize(_vec_lengths["DECAY_CONSTANT"]);

  switch (_interp_type)
  {
    case LSQ:
      leastSquaresComputeQpProperties();
      break;
    case SPLINE:
      splineComputeQpProperties();
      break;
    case MONOTONE_CUBIC:
      monotoneCubicComputeQpProperties();
      break;
    case BICUBIC:
      bicubicSplineComputeQpProperties();
      break;
    case LINEAR:
      linearComputeQpProperties();
      break;
    case NONE:
      dummyComputeQpProperties();
      break;
  }

  if (_perform_control && _peak_power_density > _peak_power_density_set_point)
    for (unsigned i = 0; i < _num_groups; ++i)
      _remxs[_qp][i] += _controller_gain * (_peak_power_density - _peak_power_density_set_point);
}
