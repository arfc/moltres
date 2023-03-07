#include "GenericMoltresMaterial.h"
#include "MooseUtils.h"

// #define PRINT(var) #var

registerMooseObject("MoltresApp", GenericMoltresMaterial);

InputParameters
GenericMoltresMaterial::validParams()
{
  InputParameters params = NuclearMaterial::validParams();
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

  _file_map["REMXS"] = "REMXS";
  _file_map["NSF"] = "NSF";
  _file_map["DIFFCOEF"] = "DIFFCOEF";
  _file_map["BETA_EFF"] = "BETA_EFF";
  if (getParam<bool>("sss2_input"))
  {
    _file_map["FISSXS"] = "FISS";
    _file_map["FISSE"] = "KAPPA";
    _file_map["RECIPVEL"] = "INVV";
    _file_map["CHI_T"] = "CHIT";
    _file_map["CHI_P"] = "CHIP";
    _file_map["CHI_D"] = "CHID";
    _file_map["GTRANSFXS"] = "SP0";
    _file_map["DECAY_CONSTANT"] = "LAMBDA";
  }
  else
  {
    _file_map["FISSXS"] = "FISSXS";
    _file_map["FISSE"] = "FISSE";
    _file_map["RECIPVEL"] = "RECIPVEL";
    _file_map["CHI_T"] = "CHI";
    _file_map["CHI_P"] = "CHI";
    _file_map["CHI_D"] = "CHI_D";
    _file_map["GTRANSFXS"] = "GTRANSFXS";
    _file_map["DECAY_CONSTANT"] = "DECAY_CONSTANT";
  }
  switch (_interp_type)
  {
    case LSQ:
      leastSquaresConstruct(property_tables_root);
      break;
    case SPLINE:
      Construct(property_tables_root);
      break;
    case MONOTONE_CUBIC:
      Construct(property_tables_root);
      break;
    case BICUBIC:
      bicubicSplineConstruct(property_tables_root, parameters);
      break;
    case LINEAR:
      Construct(property_tables_root);
      break;
    case NONE:
      Construct(property_tables_root);
      break;
    default:
      mooseError("Invalid enum type for interp_type");
      break;
  }
}

void
GenericMoltresMaterial::Construct(std::string & property_tables_root)
{
  Real value;
  int tempLength = 0;
  bool onewarn = false;
  std::vector<Real> oldtemperature;
  for (decltype(_xsec_names.size()) j = 0; j < _xsec_names.size(); ++j)
  {
    std::vector<Real> temperature;
    std::string file_name = property_tables_root + _file_map[_xsec_names[j]] + ".txt";
    int num_lines = 0;
    std::string line;
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());
    auto o = _vec_lengths[_xsec_names[j]];
    _xsec_map[_xsec_names[j]].resize(o);
    _xsec_linear_interpolators[_xsec_names[j]].resize(o);
    _xsec_spline_interpolators[_xsec_names[j]].resize(o);
    _xsec_monotone_cubic_interpolators[_xsec_names[j]].resize(o);

    if (_xsec_names[j]=="CHI_D" and not myfile.good())
    {
      //chi_d backwards compatibility on unit tests:
      for (decltype(o) k = 0; k < o; ++k)
        for (int i = 0; i < tempLength; ++i)
          _xsec_map["CHI_D"][k].push_back(0.0);

      for (int i = 0; i < tempLength; ++i)
        // o!=0 avoids segfault for prec-only problems (diracHX test)
        if (o != 0)
          _xsec_map["CHI_D"][0][i] = 1.0;

      if (!onewarn)
      {
        mooseWarning("CHI_D data missing -> assume delayed neutrons born in top group for material" + _name);
        onewarn = true;
      }
      switch (_interp_type)
      {
        case NONE:
          break;
        case LINEAR:
          for (decltype(o) k = 0; k < o; ++k)
            _xsec_linear_interpolators[_xsec_names[j]][k].setData(oldtemperature,
                                                                  _xsec_map["CHI_D"][k]);
          break;
        case SPLINE:
          for (decltype(o) k = 0; k < o; ++k)
            _xsec_spline_interpolators[_xsec_names[j]][k].setData(oldtemperature,
                                                                  _xsec_map["CHI_D"][k]);
          break;
        case MONOTONE_CUBIC:
          for (decltype(o) k = 0; k < o; ++k)
            _xsec_monotone_cubic_interpolators[_xsec_names[j]][k].setData(oldtemperature,
                                                                          _xsec_map["CHI_D"][k]);
          break;
      }
      continue;
    }

    while (std::getline(myfile, line))
      ++num_lines;

    myfile.clear();
    myfile.seekg(0);
    if (myfile.is_open())
    {
      while (myfile >> value)
      {
        if (!temperature.empty() && value < temperature.back())
        {
          // Check if temperature values are in increasing order. Also errors out if wrong number
          // of data values are provided.
          mooseError(
              "The temperature values in the " + _file_map[_xsec_names[j]] + " files are not in "
              "increasing order, or the number of " + _file_map[_xsec_names[j]] + " values does "
              "not match the num_groups/num_precursor_groups parameter.");
        }
        temperature.push_back(value);
        for (decltype(o) k = 0; k < o; ++k)
        {
          myfile >> value;
          if (myfile.eof())
            // Check if insufficient number of data values are provided for interp_type=none
            mooseError(
                "The number of " + _file_map[_xsec_names[j]] + " values does not match "
                "the num_groups/num_precursor_groups parameter.");
          _xsec_map[_xsec_names[j]][k].push_back(value);
        }
      }
      tempLength = temperature.size();
      if (num_lines != tempLength)
        // Catch edge cases occurring when the total number of values is divisible by an
        // erroneous num_groups/num_precursor_groups parameter.
        mooseError("The number of " + _file_map[_xsec_names[j]] + " values does not match "
                   "the num_groups/num_precursor_groups parameter.");
      oldtemperature = temperature;
      switch (_interp_type)
      {
        case NONE:
          if (tempLength > 1)
            // Reject if group constants provided at multiple temperatures for interp_type=none"
            mooseError(
                _file_map[_xsec_names[j]] + " values provided at multiple temperatures with "
                "interp_type=none. Remove extra temperature data or change interpolation scheme.");
          break;
        case LINEAR:
          for (decltype(o) k = 0; k < o; ++k)
          {
            _xsec_linear_interpolators[_xsec_names[j]][k].setData(temperature,
                                                                  _xsec_map[_xsec_names[j]][k]);
          }
          break;
        case SPLINE:
          for (decltype(o) k = 0; k < o; ++k)
          {
            _xsec_spline_interpolators[_xsec_names[j]][k].setData(temperature,
                                                                  _xsec_map[_xsec_names[j]][k]);
          }
          break;
        case MONOTONE_CUBIC:
          if (tempLength < 3)
            mooseError("Monotone cubic interpolation requires at least three data points.");
          for (decltype(o) k = 0; k < o; ++k)
          {
            _xsec_monotone_cubic_interpolators[_xsec_names[j]][k].setData(
                temperature,
                _xsec_map[_xsec_names[j]][k]);
          }
          break;
      }
      myfile.close();
    }
    else
      mooseError("Unable to open file " + file_name);
  }
}

void
GenericMoltresMaterial::bicubicSplineConstruct(std::string & property_tables_root,
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

  for (decltype(_xsec_names.size()) j = 0; j < _xsec_names.size(); ++j)
  {
    std::string file_name = property_tables_root + _file_map[_xsec_names[j]] + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile(file_name_ref.c_str());

    auto o = _vec_lengths[_xsec_names[j]];

    std::map<std::string, std::vector<std::vector<std::vector<Real>>>> bicubic_xsec_map;
    bicubic_xsec_map[_xsec_names[j]].resize(o);
    for (decltype(o) k = 0; k < o; ++k)
      bicubic_xsec_map[_xsec_names[j]][k].resize(l);
    _xsec_bicubic_spline_interpolators[_xsec_names[j]].resize(o);
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
            bicubic_xsec_map[_xsec_names[j]][k][h].push_back(value);
          }
        }
      }
      myfile.close();
      for (decltype(o) k = 0; k < o; ++k)
        _xsec_bicubic_spline_interpolators[_xsec_names[j]][k].setData(
            fuel_temperature, mod_temperature, bicubic_xsec_map[_xsec_names[j]][k]);
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
GenericMoltresMaterial::leastSquaresConstruct(std::string & property_tables_root)
{
  Real value;

  // loop over type of constant, e.g. remxs, diffcoeff, etc.
  for (decltype(_xsec_names.size()) i = 0; i < _xsec_names.size(); ++i)
  {
    _xsec_map[_xsec_names[i]].resize(2);
    // loop over number of constants in least squares fit (2 for linear)
    for (unsigned int j = 0; j <= 1; ++j)
      _xsec_map[_xsec_names[i]][j].resize(_vec_lengths[_xsec_names[i]]);
  }

  std::string file_name = property_tables_root;
  const std::string & file_name_ref = file_name;
  std::ifstream myfile(file_name_ref.c_str());

  auto m = _xsec_names.size();
  if (myfile.is_open())
  {
    // loop over type of constant, e.g. remxs, diffcoeff, etc.
    for (decltype(m) i = 0; i < m; ++i)
    {
      // loop over number of groups / number of precursor groups (or number of groups squared for
      // GTRANSFXS
      auto n = _vec_lengths[_xsec_names[i]];
      for (decltype(n) j = 0; j < n; ++j)
      {
        // loop over number of constants in least squares fit (2 for linear)
        for (unsigned int k = 0; k <= 1; ++k)
        {
          myfile >> value;
          _xsec_map[_xsec_names[i]][k][j] = value;
        }
      }
    }
  }

  _remxs_consts = _xsec_map["REMXS"];
  _fissxs_consts = _xsec_map["FISSXS"];
  _nsf_consts = _xsec_map["NSF"];
  _fisse_consts = _xsec_map["FISSE"];
  _diffcoeff_consts = _xsec_map["DIFFCOEF"];
  _recipvel_consts = _xsec_map["RECIPVEL"];
  _chi_t_consts = _xsec_map["CHI_T"];
  _chi_p_consts = _xsec_map["CHI_P"];
  _chi_d_consts = _xsec_map["CHI_D"];
  _gtransfxs_consts = _xsec_map["GTRANSFXS"];
  _beta_eff_consts = _xsec_map["BETA_EFF"];
  _decay_constants_consts = _xsec_map["DECAY_CONSTANT"];
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
    _chi_t[_qp][i] =
        _xsec_bicubic_spline_interpolators["CHI_T"][i].sample(_temperature[_qp], _other_temp);
    _chi_p[_qp][i] =
        _xsec_bicubic_spline_interpolators["CHI_P"][i].sample(_temperature[_qp], _other_temp);
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
    _d_chi_t_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI_T"][i].sampleDerivative(
        _temperature[_qp], _other_temp, 1);
    _d_chi_p_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI_P"][i].sampleDerivative(
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
    _chi_t[_qp][i] =
        _xsec_bicubic_spline_interpolators["CHI_T"][i].sample(_other_temp, _temperature[_qp]);
    _chi_p[_qp][i] =
        _xsec_bicubic_spline_interpolators["CHI_P"][i].sample(_other_temp, _temperature[_qp]);
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
    _d_chi_t_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI_T"][i].sampleDerivative(
        _other_temp, _temperature[_qp], 2);
    _d_chi_p_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI_P"][i].sampleDerivative(
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
    _chi_t[_qp][i] = _chi_t_consts[0][i] * _temperature[_qp] + _chi_t_consts[1][i];
    _chi_p[_qp][i] = _chi_p_consts[0][i] * _temperature[_qp] + _chi_p_consts[1][i];
    _chi_d[_qp][i] = _chi_d_consts[0][i] * _temperature[_qp] + _chi_d_consts[1][i];
    _d_remxs_d_temp[_qp][i] = _remxs_consts[0][i];
    _d_fissxs_d_temp[_qp][i] = _fissxs_consts[0][i];
    _d_nsf_d_temp[_qp][i] = _nsf_consts[0][i];
    _d_fisse_d_temp[_qp][i] = _fisse_consts[0][i] * 1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = _diffcoeff_consts[0][i];
    _d_recipvel_d_temp[_qp][i] = _recipvel_consts[0][i];
    _d_chi_t_d_temp[_qp][i] = _chi_t_consts[0][i];
    _d_chi_p_d_temp[_qp][i] = _chi_p_consts[0][i];
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
  NuclearMaterial::preComputeQpProperties();

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
