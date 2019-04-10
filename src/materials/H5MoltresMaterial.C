#include "H5MoltresMaterial.h"
#include "MooseUtils.h"

// #define PRINT(var) #var

registerMooseObject("MoltresApp", H5MoltresMaterial);

template <>
InputParameters
validParams<H5MoltresMaterial>()
{
  InputParameters params = validParams<GenericConstantMaterial>();
  params.addRequiredParam<std::string>("base_file", "The file containing macroscopic XS.");
  params.addRequiredParam<std::string>("material_key",
                                       "The file key where the macroscopic XS can be found.");
  params.addRequiredParam<unsigned>("num_groups",
                                    "The number of groups the energy spectrum is divided into.");
  params.addRequiredParam<unsigned>("num_precursor_groups",
                                    "The number of delayed neutron precursor groups.");
  params.addCoupledVar(
      "temperature", 900, "The temperature field for determining group constants.");
  params.addRequiredParam<MooseEnum>(
      "interp_type", H5MoltresMaterial::interpTypes(), "The type of interpolation to perform.");
  return params;
}

H5MoltresMaterial::H5MoltresMaterial(const InputParameters & parameters)
  : GenericConstantMaterial(parameters),
    _temperature(coupledValue("temperature")),
    _remxs(declareProperty<std::vector<Real>>("remxs")),
    _fissxs(declareProperty<std::vector<Real>>("fissxs")),
    _nsf(declareProperty<std::vector<Real>>("nsf")),
    _fisse(declareProperty<std::vector<Real>>("fisse")),
    _diffcoef(declareProperty<std::vector<Real>>("diffcoef")),
    _recipvel(declareProperty<std::vector<Real>>("recipvel")),
    _chi(declareProperty<std::vector<Real>>("chi")),
    _chi_d(declareProperty<std::vector<Real>>("chi_d")),
    _gtransfxs(declareProperty<std::vector<Real>>("gtransfxs")),
    _beta_eff(declareProperty<std::vector<Real>>("beta_eff")),
    _beta(declareProperty<Real>("beta")), // Why is this here
    _decay_constant(declareProperty<std::vector<Real>>("decay_constant")),

    _d_remxs_d_temp(declareProperty<std::vector<Real>>("d_remxs_d_temp")),
    _d_fissxs_d_temp(declareProperty<std::vector<Real>>("d_fissxs_d_temp")),
    _d_nsf_d_temp(declareProperty<std::vector<Real>>("d_nsf_d_temp")),
    _d_fisse_d_temp(declareProperty<std::vector<Real>>("d_fisse_d_temp")),
    _d_diffcoef_d_temp(declareProperty<std::vector<Real>>("d_diffcoef_d_temp")),
    _d_recipvel_d_temp(declareProperty<std::vector<Real>>("d_recipvel_d_temp")),
    _d_chi_d_temp(declareProperty<std::vector<Real>>("d_chi_d_temp")),
    _d_chi_d_d_temp(declareProperty<std::vector<Real>>("d_chi_d_temp")),
    _d_gtransfxs_d_temp(declareProperty<std::vector<Real>>("d_gtransfxs_d_temp")),
    _d_beta_eff_d_temp(declareProperty<std::vector<Real>>("d_beta_eff_d_temp")),
    _d_beta_d_temp(declareProperty<Real>("d_beta_d_temp")), // why is this here
    _d_decay_constant_d_temp(declareProperty<std::vector<Real>>("d_decay_constant_d_temp")),
    _interp_type(getParam<MooseEnum>("interp_type"))
{
  _num_groups = getParam<unsigned>("num_groups");
  _num_precursor_groups = getParam<unsigned>("num_precursor_groups");
  std::string base_file = getParam<std::string>("base_file");
  _material_key = getParam<std::string>("material_key");
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

  H5std_string FILE_NAME(base_file);
  H5::H5File file_open(FILE_NAME, H5F_ACC_RDWR);
  if (!pathExists(file_open.getId(), _material_key + "/temp"))
    mooseError("Unable to open database " + _material_key + "/temp");
  H5std_string DATASET_NAME(_material_key + "/temp");
  H5::DataSet dataset = file_open.openDataSet(DATASET_NAME);
  H5::DataSpace filespace = dataset.getSpace();
  int rank = filespace.getSimpleExtentNdims();
  hsize_t dims[rank];
  rank = filespace.getSimpleExtentDims(dims);
  double data_out[dims[0]];
  dataset.read(data_out, H5::PredType::NATIVE_DOUBLE);

  _XsTemperature.resize(dims[0]);
  for (int k = 0; k < dims[0]; ++k)
  {
    _XsTemperature[k] = data_out[k];
  }

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
  _file_map["CHI"] = "CHI";
  _file_map["CHI_D"] = "CHI_D";
  _file_map["GTRANSFXS"] = "GTRANSFXS";
  _file_map["DECAY_CONSTANT"] = "DECAY_CONSTANT";

  Construct(base_file, xsec_names);
}

bool
H5MoltresMaterial::pathExists(hid_t id, const std::string & path)
{
  return H5Lexists(id, path.c_str(), H5P_DEFAULT) > 0;
}

void
H5MoltresMaterial::Construct(std::string & base_file, std::vector<std::string> xsec_names)
{
  auto xsec_interpolators = _xsec_linear_interpolators;
  for (decltype(xsec_names.size()) j = 0; j < xsec_names.size(); ++j)
  {
    auto o = _vec_lengths[xsec_names[j]];
    auto L = _XsTemperature.size();
    std::map<std::string, std::vector<std::vector<Real>>> xsec_map;
    xsec_map[xsec_names[j]].resize(o);

    H5std_string FILE_NAME(base_file);
    H5::H5File file_open(FILE_NAME, H5F_ACC_RDWR);
    for (decltype(_XsTemperature.size()) l = 0; l < L; ++l)
    {
      H5::DataSet dataset;
      bool flag =
          pathExists(file_open.getId(),
                     _material_key + "/" + std::to_string(static_cast<int>(_XsTemperature[l])) +
                         "/" + _file_map[xsec_names[j]]);
      if (xsec_names[j] == "CHI_D" && !flag)
      {
        for (decltype(_num_groups) k = 0; k < _num_groups; ++k)
          if (_num_groups != 0)
            xsec_map["CHI_D"][k].push_back(0.0);
        xsec_map["CHI_D"][0][-1] = 1.0;
        mooseWarning(
            "CHI_D data missing -> assume delayed neutrons born in top group for material" + _name);
        continue;
      }
      if (!flag)
        mooseError("Unable to open database " + _material_key + "/" +
                   std::to_string(static_cast<int>(_XsTemperature[l])) + "/" +
                   _file_map[xsec_names[j]]);

      H5std_string DATASET_NAME(_material_key + "/" +
                                std::to_string(static_cast<int>(_XsTemperature[l])) + "/" +
                                _file_map[xsec_names[j]]);
      dataset = file_open.openDataSet(DATASET_NAME);

      H5::DataSpace filespace = dataset.getSpace();
      int rank = filespace.getSimpleExtentNdims();
      hsize_t dims[rank];
      rank = filespace.getSimpleExtentDims(dims);
      if (o != dims[0])
        mooseError("Dimensions of " + _material_key + "/" + std::to_string(_XsTemperature[l]) +
                   "/" + _file_map[xsec_names[j]] + " and num_groups do not match\n" +
                   std::to_string(dims[0]) + "!=" + std::to_string(o));
      double data_out[dims[0]];
      dataset.read(data_out, H5::PredType::NATIVE_DOUBLE);

      for (decltype(_num_groups) k = 0; k < o; ++k)
      {
        xsec_map[xsec_names[j]][k].push_back(data_out[k]);
      }
    }
    switch (_interp_type)
    {
      case NONE:
        for (decltype(_num_groups) k = 0; k < o; ++k)
        {
          _xsec_linear_interpolators[xsec_names[j]][k].setData(_XsTemperature,
                                                               xsec_map[xsec_names[j]][k]);
        }
        break;
      case LINEAR:
        for (decltype(_num_groups) k = 0; k < o; ++k)
        {
          _xsec_linear_interpolators[xsec_names[j]][k].setData(_XsTemperature,
                                                               xsec_map[xsec_names[j]][k]);
        }
        break;
      case SPLINE:
        for (decltype(_num_groups) k = 0; k < o; ++k)
        {
          _xsec_spline_interpolators[xsec_names[j]][k].setData(_XsTemperature,
                                                               xsec_map[xsec_names[j]][k]);
        }
        break;
      case MONOTONE_CUBIC:
        for (decltype(_num_groups) k = 0; k < o; ++k)
        {
          _xsec_monotone_cubic_interpolators[xsec_names[j]][k].setData(_XsTemperature,
                                                                       xsec_map[xsec_names[j]][k]);
        }
        break;
    }
  }
}

void
H5MoltresMaterial::dummyComputeQpProperties()
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
    _chi[_qp][i] = _xsec_linear_interpolators["CHI"][i].sample(dummy_temp);
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
    _d_chi_d_temp[_qp][i] = _xsec_linear_interpolators["CHI"][i].sampleDerivative(dummy_temp);
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
H5MoltresMaterial::splineComputeQpProperties()
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
    _d_chi_d_temp[_qp][i] =
        _xsec_spline_interpolators["CHI"][i].sampleDerivative(_temperature[_qp]);
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
H5MoltresMaterial::monotoneCubicComputeQpProperties()
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
    _d_chi_d_temp[_qp][i] =
        _xsec_monotone_cubic_interpolators["CHI"][i].sampleDerivative(_temperature[_qp]);
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
H5MoltresMaterial::linearComputeQpProperties()
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
    _d_chi_d_temp[_qp][i] =
        _xsec_linear_interpolators["CHI"][i].sampleDerivative(_temperature[_qp]);
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
H5MoltresMaterial::computeQpProperties()
{
  for (unsigned int i = 0; i < _num_props; i++)
    (*_properties[i])[_qp] = _prop_values[i];
  // Why isnt this done at begining???
  _remxs[_qp].resize(_num_groups);
  _fissxs[_qp].resize(_num_groups);
  _nsf[_qp].resize(_num_groups);
  _fisse[_qp].resize(_num_groups);
  _diffcoef[_qp].resize(_num_groups);
  _recipvel[_qp].resize(_num_groups);
  _chi[_qp].resize(_num_groups);
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
  _d_chi_d_temp[_qp].resize(_num_groups);
  _d_chi_d_d_temp[_qp].resize(_num_groups);

  _d_gtransfxs_d_temp[_qp].resize(_num_groups * _num_groups);

  _d_beta_eff_d_temp[_qp].resize(_num_precursor_groups);
  _d_decay_constant_d_temp[_qp].resize(_num_precursor_groups);

  switch (_interp_type)
  {
    case NONE:
      dummyComputeQpProperties();
      break;
    case LINEAR:
      linearComputeQpProperties();
      break;
    case SPLINE:
      linearComputeQpProperties();
      break;
    case MONOTONE_CUBIC:
      monotoneCubicComputeQpProperties();
      break;
  }
}
