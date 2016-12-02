#include "GenericMoltresMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

template<>
InputParameters validParams<GenericMoltresMaterial>()
{
  InputParameters params = validParams<GenericConstantMaterial>();
  params.addRequiredParam<std::string>("property_tables_root", "The file root name containing interpolation tables for material properties.");
  params.addRequiredParam<int>("num_groups", "The number of groups the energy spectrum is divided into.");
  params.addRequiredParam<int>("num_precursor_groups", "The number of delayed neutron precursor groups.");
  params.addCoupledVar("temperature", 937, "The temperature field for determining group constants.");
  params.addParam<bool>("bivariable_interp", false, "Whether to use both fuel and moderator temperatures to interpolate the material properties.");
  params.addParam<std::vector<Real> >("fuel_temp_points", "The fuel temperature interpolation points.");
  params.addParam<std::vector<Real> >("mod_temp_points", "The moderator temperature interpolation points.");
  params.addParam<PostprocessorName>("other_temp", 0, "If doing bivariable interpolation, need to supply a postprocessor for the average temperature of the other material.");
  params.addParam<std::string>("material", "Must specify either *fuel* or *moderator*.");
  params.addRequiredParam<bool>("monotonic_interpolation", "If this option is true, neutronic properties are fit with a least squared linear fit.");
  return params;
}


GenericMoltresMaterial::GenericMoltresMaterial(const InputParameters & parameters) :
    GenericConstantMaterial(parameters),
    _temperature(coupledValue("temperature")),
    _remxs(declareProperty<std::vector<Real> >("remxs")),
    _fissxs(declareProperty<std::vector<Real> >("fissxs")),
    _nsf(declareProperty<std::vector<Real> >("nsf")),
    _fisse(declareProperty<std::vector<Real> >("fisse")),
    _diffcoef(declareProperty<std::vector<Real> >("diffcoef")),
    _recipvel(declareProperty<std::vector<Real> >("recipvel")),
    _chi(declareProperty<std::vector<Real> >("chi")),
    _gtransfxs(declareProperty<std::vector<Real> >("gtransfxs")),
    _beta_eff(declareProperty<std::vector<Real> >("beta_eff")),
    _decay_constant(declareProperty<std::vector<Real> >("decay_constant")),

    _d_remxs_d_temp(declareProperty<std::vector<Real> >("d_remxs_d_temp")),
    _d_fissxs_d_temp(declareProperty<std::vector<Real> >("d_fissxs_d_temp")),
    _d_nsf_d_temp(declareProperty<std::vector<Real> >("d_nsf_d_temp")),
    _d_fisse_d_temp(declareProperty<std::vector<Real> >("d_fisse_d_temp")),
    _d_diffcoef_d_temp(declareProperty<std::vector<Real> >("d_diffcoef_d_temp")),
    _d_recipvel_d_temp(declareProperty<std::vector<Real> >("d_recipvel_d_temp")),
    _d_chi_d_temp(declareProperty<std::vector<Real> >("d_chi_d_temp")),
    _d_gtransfxs_d_temp(declareProperty<std::vector<Real> >("d_gtransfxs_d_temp")),
    _d_beta_eff_d_temp(declareProperty<std::vector<Real> >("d_beta_eff_d_temp")),
    _d_decay_constant_d_temp(declareProperty<std::vector<Real> >("d_decay_constant_d_temp")),
    _bivariable_interp(getParam<bool>("bivariable_interp")),
    _other_temp(getPostprocessorValue("other_temp")),
    _monotonic_interpolation(getParam<bool>("monotonic_interpolation"))
{
  std::string property_tables_root = getParam<std::string>("property_tables_root");
  _num_groups = getParam<int>("num_groups");
  _num_precursor_groups = getParam<int>("num_precursor_groups");

  if (_monotonic_interpolation)
  {
    std::vector<std::vector<std::vector<Real> > > nt_consts(8);
    nt_consts[0] = _flux_consts;
    nt_consts[1] = _remxs_consts;
    nt_consts[2] = _fissxs_consts;
    nt_consts[3] = _nubar_consts;
    nt_consts[4] = _nsf_consts;
    nt_consts[5] = _fisse_consts;
    nt_consts[6] = _diffcoeff_consts;
    nt_consts[7] = _recipvel_consts;


    for (decltype(nt_consts.size()) i = 0; i < nt_consts.size(); ++i)
      for (decltype(nt_consts[i].size()) j = 0; j < nt_consts[i].size(); ++j)
        nt_consts[i][j].resize(_num_groups);

    std::string file_name = property_tables_root;
    const std::string & file_name_ref = file_name;
    std::ifstream myfile (file_name_ref.c_str());
    Real value;
    if (myfile.is_open())
    {
      // loop over type of constant, e.g. remxs, diffcoeff, etc.
      for (decltype(nt_consts.size()) i = 0; i < nt_consts.size(); ++i)
        // loop over number of groups
        for (decltype(_num_groups) j = 0; j < _num_groups; ++j)
          // loop over number of constants in least squares fit (2 for linear)
          for (decltype(nt_consts[i].size()) k = 0; k < nt_consts[i].size(); ++k)
          {
            myfile >> value;
            nt_consts[i][k][j] = value;
          }
    }
  }

  else
  {
    Real value;

    std::string property_tables_root = getParam<std::string>("property_tables_root");
    std::vector<std::string> xsec_names {"FLUX", "REMXS", "FISSXS", "NUBAR", "NSF", "FISSE", "DIFFCOEF",
        "RECIPVEL", "CHI", "GTRANSFXS", "BETA_EFF", "DECAY_CONSTANT"};
    auto n = xsec_names.size();

    if (!_bivariable_interp)
    {
      for (decltype(n) j = 0; j < n; ++j)
      {
        std::vector<Real> temperature;
        std::string file_name = property_tables_root + xsec_names[j] + ".txt";
        const std::string & file_name_ref = file_name;
        std::ifstream myfile (file_name_ref.c_str());

        if (xsec_names[j].compare("GTRANSFXS") == 0)
          _vec_lengths[xsec_names[j]] = _num_groups * _num_groups;
        else if (xsec_names[j].compare("BETA_EFF") == 0 || xsec_names[j].compare("DECAY_CONSTANT") == 0)
          _vec_lengths[xsec_names[j]] = _num_precursor_groups;
        else
          _vec_lengths[xsec_names[j]] = _num_groups;
        auto o = _vec_lengths[xsec_names[j]];

        std::map<std::string, std::vector<std::vector<Real> > > xsec_map;
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
            _xsec_spline_interpolators[xsec_names[j]][k].setData(temperature, xsec_map[xsec_names[j]][k]);
        }
        else
          mooseError("Unable to open file " << file_name);
      }
    }
    else
    {
      std::vector<Real> fuel_temperature;
      std::vector<Real> mod_temperature;
      if (isParamValid("fuel_temp_points") && !(getParam<std::vector<Real> >("fuel_temp_points").empty()))
        fuel_temperature = getParam<std::vector<Real> >("fuel_temp_points");
      else
        mooseError("You forgot to supply *fuel_temp_points* for interpolation!");
      if (isParamValid("mod_temp_points") && !(getParam<std::vector<Real> >("mod_temp_points").empty()))
        mod_temperature = getParam<std::vector<Real> >("mod_temp_points");
      else
        mooseError("You forgot to supply *mod_temp_points* for interpolation!");
      auto l = fuel_temperature.size();
      auto m = mod_temperature.size();
      auto lm = l * m;
      decltype(lm) inc = 0;

      for (decltype(n) j = 0; j < n; ++j)
      {
        std::string file_name = property_tables_root + xsec_names[j] + ".txt";
        const std::string & file_name_ref = file_name;
        std::ifstream myfile (file_name_ref.c_str());
        if (xsec_names[j].compare("GTRANSFXS") == 0)
          _vec_lengths[xsec_names[j]] = _num_groups * _num_groups;
        else if (xsec_names[j].compare("BETA_EFF") == 0 || xsec_names[j].compare("DECAY_CONSTANT") == 0)
          _vec_lengths[xsec_names[j]] = _num_precursor_groups;
        else
          _vec_lengths[xsec_names[j]] = _num_groups;
        auto o = _vec_lengths[xsec_names[j]];

        std::map<std::string, std::vector<std::vector<std::vector<Real> > > > xsec_map;
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
            _xsec_bicubic_spline_interpolators[xsec_names[j]][k].setData(fuel_temperature, mod_temperature, xsec_map[xsec_names[j]][k]);
        }
        else
          mooseError("Unable to open file " << file_name);
      }
    }

    if (_bivariable_interp)
    {
      if (!parameters.isParamSetByUser("other_temp"))
        mooseError("If doing bivariable interpolation, you must supply a postprocessor value for the average temperature of the other material.");

      if (!isParamValid("material"))
        mooseError("User must specify whether we are in the *fuel* or *moderator*.");
      else
      {
        _material = getParam<std::string>("material");
        if (_material.compare("fuel") != 0 && _material.compare("moderator") != 0)
          mooseError("Only valid choices for material parameter are *fuel* and *moderator*.");
      }
    }
  }
}

void
GenericMoltresMaterial::computeQpProperties()
{
  for (unsigned int i=0; i<_num_props; i++)
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

  if (!_bivariable_interp)
  {
    for (int i = 0; i < _num_groups; ++i)
    {
      _remxs[_qp][i] = _xsec_spline_interpolators["REMXS"][i].sample(_temperature[_qp]);
      _fissxs[_qp][i] = _xsec_spline_interpolators["FISSXS"][i].sample(_temperature[_qp]);
      _nsf[_qp][i] = _xsec_spline_interpolators["NSF"][i].sample(_temperature[_qp]);
      _fisse[_qp][i] = _xsec_spline_interpolators["FISSE"][i].sample(_temperature[_qp]) * 1e6 * 1.6e-19; // convert from MeV to Joules
      _diffcoef[_qp][i] = _xsec_spline_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
      _recipvel[_qp][i] = _xsec_spline_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
      _chi[_qp][i] = _xsec_spline_interpolators["CHI"][i].sample(_temperature[_qp]);
      _d_remxs_d_temp[_qp][i] = _xsec_spline_interpolators["REMXS"][i].sampleDerivative(_temperature[_qp]);
      _d_fissxs_d_temp[_qp][i] = _xsec_spline_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]);
      _d_nsf_d_temp[_qp][i] = _xsec_spline_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]);
      _d_fisse_d_temp[_qp][i] = _xsec_spline_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]) * 1e6 * 1.6e-19; // convert from MeV to Joules
      _d_diffcoef_d_temp[_qp][i] = _xsec_spline_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]);
      _d_recipvel_d_temp[_qp][i] = _xsec_spline_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]);
      _d_chi_d_temp[_qp][i] = _xsec_spline_interpolators["CHI"][i].sampleDerivative(_temperature[_qp]);
    }
    for (int i = 0; i < _num_groups * _num_groups; ++i)
    {
      _gtransfxs[_qp][i] = _xsec_spline_interpolators["GTRANSFXS"][i].sample(_temperature[_qp]);
      _d_gtransfxs_d_temp[_qp][i] = _xsec_spline_interpolators["GTRANSFXS"][i].sampleDerivative(_temperature[_qp]);
    }
    for (int i = 0; i < _num_precursor_groups; ++i)
    {
      _beta_eff[_qp][i] = _xsec_spline_interpolators["BETA_EFF"][i].sample(_temperature[_qp]);
      _d_beta_eff_d_temp[_qp][i] = _xsec_spline_interpolators["BETA_EFF"][i].sampleDerivative(_temperature[_qp]);
      _decay_constant[_qp][i] = _xsec_spline_interpolators["DECAY_CONSTANT"][i].sample(_temperature[_qp]);
      _d_decay_constant_d_temp[_qp][i] = _xsec_spline_interpolators["DECAY_CONSTANT"][i].sampleDerivative(_temperature[_qp]);
    }
  }
  else
  {
    if (_material.compare("fuel") == 0)
    {
      for (int i = 0; i < _num_groups; ++i)
      {
        _remxs[_qp][i] = _xsec_bicubic_spline_interpolators["REMXS"][i].sample(_temperature[_qp], _other_temp);
        _fissxs[_qp][i] = _xsec_bicubic_spline_interpolators["FISSXS"][i].sample(_temperature[_qp], _other_temp);
        _nsf[_qp][i] = _xsec_bicubic_spline_interpolators["NSF"][i].sample(_temperature[_qp], _other_temp);
        _fisse[_qp][i] = _xsec_bicubic_spline_interpolators["FISSE"][i].sample(_temperature[_qp], _other_temp) * 1e6 * 1.6e-19; // convert from MeV to Joules
        _diffcoef[_qp][i] = _xsec_bicubic_spline_interpolators["DIFFCOEF"][i].sample(_temperature[_qp], _other_temp);
        _recipvel[_qp][i] = _xsec_bicubic_spline_interpolators["RECIPVEL"][i].sample(_temperature[_qp], _other_temp);
        _chi[_qp][i] = _xsec_bicubic_spline_interpolators["CHI"][i].sample(_temperature[_qp], _other_temp);
        _d_remxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["REMXS"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
        _d_fissxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
        _d_nsf_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["NSF"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
        _d_fisse_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp], _other_temp, 1) * 1e6 * 1.6e-19; // convert from MeV to Joules
        _d_diffcoef_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
        _d_recipvel_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
        _d_chi_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
      }
      for (int i = 0; i < _num_groups * _num_groups; ++i)
      {
        _gtransfxs[_qp][i] = _xsec_bicubic_spline_interpolators["GTRANSFXS"][i].sample(_temperature[_qp], _other_temp);
        _d_gtransfxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["GTRANSFXS"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
      }
      for (int i = 0; i < _num_precursor_groups; ++i)
      {
        _beta_eff[_qp][i] = _xsec_bicubic_spline_interpolators["BETA_EFF"][i].sample(_temperature[_qp], _other_temp);
        _d_beta_eff_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["BETA_EFF"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
        _decay_constant[_qp][i] = _xsec_bicubic_spline_interpolators["DECAY_CONSTANT"][i].sample(_temperature[_qp], _other_temp);
        _d_decay_constant_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["DECAY_CONSTANT"][i].sampleDerivative(_temperature[_qp], _other_temp, 1);
      }
    }
    else if (_material.compare("moderator") == 0)
    {
      for (int i = 0; i < _num_groups; ++i)
      {
        _remxs[_qp][i] = _xsec_bicubic_spline_interpolators["REMXS"][i].sample(_other_temp, _temperature[_qp]);
        _fissxs[_qp][i] = _xsec_bicubic_spline_interpolators["FISSXS"][i].sample(_other_temp, _temperature[_qp]);
        _nsf[_qp][i] = _xsec_bicubic_spline_interpolators["NSF"][i].sample(_other_temp, _temperature[_qp]);
        _fisse[_qp][i] = _xsec_bicubic_spline_interpolators["FISSE"][i].sample(_other_temp, _temperature[_qp]) * 1e6 * 1.6e-19; // convert from MeV to Joules
        _diffcoef[_qp][i] = _xsec_bicubic_spline_interpolators["DIFFCOEF"][i].sample(_other_temp, _temperature[_qp]);
        _recipvel[_qp][i] = _xsec_bicubic_spline_interpolators["RECIPVEL"][i].sample(_other_temp, _temperature[_qp]);
        _chi[_qp][i] = _xsec_bicubic_spline_interpolators["CHI"][i].sample(_other_temp, _temperature[_qp]);
        _d_remxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["REMXS"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
        _d_fissxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["FISSXS"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
        _d_nsf_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["NSF"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
        _d_fisse_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["FISSE"][i].sampleDerivative(_other_temp, _temperature[_qp], 2) * 1e6 * 1.6e-19; // convert from MeV to Joules
        _d_diffcoef_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["DIFFCOEF"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
        _d_recipvel_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["RECIPVEL"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
        _d_chi_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["CHI"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
      }
      for (int i = 0; i < _num_groups * _num_groups; ++i)
      {
        _gtransfxs[_qp][i] = _xsec_bicubic_spline_interpolators["GTRANSFXS"][i].sample(_other_temp, _temperature[_qp]);
        _d_gtransfxs_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["GTRANSFXS"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
      }
      for (int i = 0; i < _num_precursor_groups; ++i)
      {
        _beta_eff[_qp][i] = _xsec_bicubic_spline_interpolators["BETA_EFF"][i].sample(_other_temp, _temperature[_qp]);
        _d_beta_eff_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["BETA_EFF"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
        _decay_constant[_qp][i] = _xsec_bicubic_spline_interpolators["DECAY_CONSTANT"][i].sample(_other_temp, _temperature[_qp]);
        _d_decay_constant_d_temp[_qp][i] = _xsec_bicubic_spline_interpolators["DECAY_CONSTANT"][i].sampleDerivative(_other_temp, _temperature[_qp], 2);
      }
    }
    else
      mooseError("Only valid choices for material parameter are *fuel* and *moderator*.");
  }
}
