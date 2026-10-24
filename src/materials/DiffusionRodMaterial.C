#include "DiffusionRodMaterial.h"
#include "Function.h"
#include "MooseUtils.h"

using MooseUtils::relativeFuzzyEqual;

registerMooseObject("MoltresApp", DiffusionRodMaterial);

InputParameters
DiffusionRodMaterial::validParams()
{
  InputParameters params = MoltresJsonMaterial::validParams();
  params.addRequiredParam<std::string>("nonrod_material_key",
                                       "The material key for the non-rod material.");
  params.addRequiredParam<FunctionName>("rod_height_func",
      "Name of function that provides the rod interface height");
  params.addParam<Real>("cusp_correction_power", 1,
      "Rod cusping correction power factor. "
      "The power to which the rod volume fraction is raised to.");
  params.addParam<Real>("cusp_correction_coefficient", 1,
      "Rod cusping correction coefficient. "
      "The coefficient to which the rod volume fraction is multiplied with.");
  return params;
}

DiffusionRodMaterial::DiffusionRodMaterial(const InputParameters & parameters)
  : MoltresJsonMaterial(parameters),
    _nonrod_material_key(getParam<std::string>("nonrod_material_key")),
    _rod_height(getFunction("rod_height_func")),
    _cusp_power(getParam<Real>("cusp_correction_power")),
    _cusp_coeff(getParam<Real>("cusp_correction_coefficient"))
{
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

  for (const auto & name : _xsec_names)
  {
    _nonrod_xsec_values.push_back(&_xsec_map["nonrod_" + name]);
    _nonrod_spline_interps.push_back(&_xsec_spline_interpolators["nonrod_" + name]);
    _nonrod_monotone_cubic_interps.push_back(&_xsec_monotone_cubic_interpolators["nonrod_" + name]);
    _nonrod_linear_interps.push_back(&_xsec_linear_interpolators["nonrod_" + name]);
  }
}

void
DiffusionRodMaterial::Construct(nlohmann::json xs_root)
{
  std::set<std::string> gc_set(_group_consts.begin(), _group_consts.end());
  bool oneInfo = false;
  std::string nr = "nonrod_";
  for (unsigned int j = 0; j < _xsec_names.size(); ++j)
  {
    auto o = _vec_lengths[_xsec_names[j]];
    auto L = _XsTemperature.size();

    _xsec_linear_interpolators[_xsec_names[j]].resize(o);
    _xsec_spline_interpolators[_xsec_names[j]].resize(o);
    _xsec_monotone_cubic_interpolators[_xsec_names[j]].resize(o);
    _xsec_map[_xsec_names[j]].resize(o);
    // Non-rod group constant interpolators
    _xsec_linear_interpolators[nr + _xsec_names[j]].resize(o);
    _xsec_spline_interpolators[nr + _xsec_names[j]].resize(o);
    _xsec_monotone_cubic_interpolators[nr + _xsec_names[j]].resize(o);
    _xsec_map[nr + _xsec_names[j]].resize(o);

    if (gc_set.find(_xsec_names[j]) != gc_set.end())
    {
      for (decltype(_XsTemperature.size()) l = 0; l < L; ++l)
      {
        auto temp_key = std::to_string(static_cast<int>(_XsTemperature[l]));
        auto dataset = xs_root[_material_key][temp_key][_xsec_names[j]];
        auto nonrod_dataset = xs_root[_nonrod_material_key][temp_key][_xsec_names[j]];
        if (_xsec_names[j] == "CHI_D" && dataset.empty())
        {
          for (decltype(_num_groups) k = 1; k < _num_groups; ++k)
          {
            _xsec_map["CHI_D"][k].push_back(0.0);
            _xsec_map[nr + "CHI_D"][k].push_back(0.0);
          }
          _xsec_map["CHI_D"][0].push_back(1.0);
          _xsec_map[nr + "CHI_D"][0].push_back(1.0);
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
        if (o != dims && o != 0)
          mooseError("The number of " + _material_key + "/" + temp_key + "/" +
                     _xsec_names[j] + " values does not match the "
                     "num_groups/num_precursor_groups parameter. " +
                     std::to_string(dims) + "!=" + std::to_string(o));
        for (auto k = 0; k < o; ++k)
        {
          _xsec_map[_xsec_names[j]][k].push_back(dataset[k].get<double>());
          _xsec_map[nr + _xsec_names[j]][k].push_back(nonrod_dataset[k].get<double>());
        }
      }
    } else
    {
      for (decltype(_XsTemperature.size()) l = 0; l < L; ++l)
        for (auto k = 0; k < o; ++k)
        {
          _xsec_map[_xsec_names[j]][k].push_back(0.);
          _xsec_map[nr + _xsec_names[j]][k].push_back(0.);
        }
    }
    switch (_interp_type)
    {
      case LSQ:
        mooseError("Least Squares not supported, please select \
          NONE, LINEAR, SPLINE, or MONOTONE_CUBIC ");
      case BICUBIC:
        mooseError("BICUBIC not supported, please select \
          NONE, LINEAR, SPLINE, or MONOTONE_CUBIC ");
      case NONE:
        if (L > 1)
            mooseError(
                "Group constant data for '" + _material_key + "' provided at "
                "multiple temperatures with interp_type=none. Remove extra temperature data or "
                "change interpolation scheme.");
        break;
      case LINEAR:
        for (auto k = 0; k < o; ++k)
        {
          _xsec_linear_interpolators[_xsec_names[j]][k].setData(_XsTemperature,
                                                                _xsec_map[_xsec_names[j]][k]);
          _xsec_linear_interpolators[nr + _xsec_names[j]][k].setData(_XsTemperature,
              _xsec_map[nr + _xsec_names[j]][k]);
        }
        break;
      case SPLINE:
        for (auto k = 0; k < o; ++k)
        {
          _xsec_spline_interpolators[_xsec_names[j]][k].setData(_XsTemperature,
                                                                _xsec_map[_xsec_names[j]][k]);
          _xsec_spline_interpolators[nr + _xsec_names[j]][k].setData(_XsTemperature,
              _xsec_map[nr + _xsec_names[j]][k]);
        }
        break;
      case MONOTONE_CUBIC:
        if (L < 3)
          mooseError("Monotone cubic interpolation requires at least three data points.");
        for (auto k = 0; k < o; ++k)
        {
          _xsec_monotone_cubic_interpolators[_xsec_names[j]][k].setData(
              _XsTemperature,
              _xsec_map[_xsec_names[j]][k]);
          _xsec_monotone_cubic_interpolators[nr + _xsec_names[j]][k].setData(
              _XsTemperature,
              _xsec_map[nr + _xsec_names[j]][k]);
        }
        break;
      default:
        mooseError("Invalid enum type for interp_type");
        break;
    }
  }
}

void
DiffusionRodMaterial::dummyComputeQpProperties()
{
  Real vol_frac = volumeFraction();
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = (*_xsec_values[REMXS])[i][0] * vol_frac +
                     (*_nonrod_xsec_values[REMXS])[i][0] * (1 - vol_frac);
    _fissxs[_qp][i] = (*_xsec_values[FISSXS])[i][0] * vol_frac +
                      (*_nonrod_xsec_values[FISSXS])[i][0] * (1 - vol_frac);
    _nsf[_qp][i] =
        (*_xsec_values[NSF])[i][0] * vol_frac + (*_nonrod_xsec_values[NSF])[i][0] * (1 - vol_frac);
    _fisse[_qp][i] = ((*_xsec_values[FISSE])[i][0] * vol_frac +
                      (*_nonrod_xsec_values[FISSE])[i][0] * (1 - vol_frac)) *
                     1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = (*_xsec_values[DIFFCOEF])[i][0] * vol_frac +
                        (*_nonrod_xsec_values[DIFFCOEF])[i][0] * (1 - vol_frac);
    _recipvel[_qp][i] = (*_xsec_values[RECIPVEL])[i][0] * vol_frac +
                        (*_nonrod_xsec_values[RECIPVEL])[i][0] * (1 - vol_frac);
    _chi_t[_qp][i] = (*_xsec_values[CHI_T])[i][0] * vol_frac +
                     (*_nonrod_xsec_values[CHI_T])[i][0] * (1 - vol_frac);
    _chi_p[_qp][i] = (*_xsec_values[CHI_P])[i][0] * vol_frac +
                     (*_nonrod_xsec_values[CHI_P])[i][0] * (1 - vol_frac);
    _chi_d[_qp][i] = (*_xsec_values[CHI_D])[i][0] * vol_frac +
                     (*_nonrod_xsec_values[CHI_D])[i][0] * (1 - vol_frac);
    _d_remxs_d_temp[_qp][i] = 0;
    _d_fissxs_d_temp[_qp][i] = 0;
    _d_nsf_d_temp[_qp][i] = 0;
    _d_fisse_d_temp[_qp][i] = 0; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] = 0;
    _d_recipvel_d_temp[_qp][i] = 0;
    _d_chi_t_d_temp[_qp][i] = 0;
    _d_chi_p_d_temp[_qp][i] = 0;
    _d_chi_d_d_temp[_qp][i] = 0;
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] = (*_xsec_values[GTRANSFXS])[i][0] * vol_frac +
                         (*_nonrod_xsec_values[GTRANSFXS])[i][0] * (1 - vol_frac);
    _d_gtransfxs_d_temp[_qp][i] = 0;
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] = (*_xsec_values[BETA_EFF])[i][0] * vol_frac +
                        (*_nonrod_xsec_values[BETA_EFF])[i][0] * (1 - vol_frac);
    _d_beta_eff_d_temp[_qp][i] = 0;
    _beta[_qp] += _beta_eff[_qp][i];
    _decay_constant[_qp][i] = (*_xsec_values[DECAY_CONSTANT])[i][0] * vol_frac +
                              (*_nonrod_xsec_values[DECAY_CONSTANT])[i][0] * (1 - vol_frac);
    _d_decay_constant_d_temp[_qp][i] = 0;
  }
}

void
DiffusionRodMaterial::splineComputeQpProperties()
{
  Real vol_frac = volumeFraction();
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = (*_spline_interps[REMXS])[i].sample(_temperature[_qp]) * vol_frac +
                     (*_nonrod_spline_interps[REMXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _fissxs[_qp][i] =
        (*_spline_interps[FISSXS])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[FISSXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _nsf[_qp][i] = (*_spline_interps[NSF])[i].sample(_temperature[_qp]) * vol_frac +
                   (*_nonrod_spline_interps[NSF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _fisse[_qp][i] =
        ((*_spline_interps[FISSE])[i].sample(_temperature[_qp]) * vol_frac +
         (*_nonrod_spline_interps[FISSE])[i].sample(_temperature[_qp]) * (1 - vol_frac)) *
        1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] =
        (*_spline_interps[DIFFCOEF])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[DIFFCOEF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _recipvel[_qp][i] =
        (*_spline_interps[RECIPVEL])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[RECIPVEL])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_t[_qp][i] = (*_spline_interps[CHI_T])[i].sample(_temperature[_qp]) * vol_frac +
                     (*_nonrod_spline_interps[CHI_T])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_p[_qp][i] = (*_spline_interps[CHI_P])[i].sample(_temperature[_qp]) * vol_frac +
                     (*_nonrod_spline_interps[CHI_P])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_d[_qp][i] = (*_spline_interps[CHI_D])[i].sample(_temperature[_qp]) * vol_frac +
                     (*_nonrod_spline_interps[CHI_D])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_remxs_d_temp[_qp][i] =
        (*_spline_interps[REMXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[REMXS])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_fissxs_d_temp[_qp][i] =
        (*_spline_interps[FISSXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[FISSXS])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_nsf_d_temp[_qp][i] =
        (*_spline_interps[NSF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[NSF])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_fisse_d_temp[_qp][i] =
        ((*_spline_interps[FISSE])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
         (*_nonrod_spline_interps[FISSE])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac)) *
        1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] =
        (*_spline_interps[DIFFCOEF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[DIFFCOEF])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_recipvel_d_temp[_qp][i] =
        (*_spline_interps[RECIPVEL])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[RECIPVEL])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_chi_t_d_temp[_qp][i] =
        (*_spline_interps[CHI_T])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[CHI_T])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_chi_p_d_temp[_qp][i] =
        (*_spline_interps[CHI_P])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[CHI_P])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_chi_d_d_temp[_qp][i] =
        (*_spline_interps[CHI_D])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[CHI_D])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] =
        (*_spline_interps[GTRANSFXS])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[GTRANSFXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_gtransfxs_d_temp[_qp][i] =
        (*_spline_interps[GTRANSFXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[GTRANSFXS])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] =
        (*_spline_interps[BETA_EFF])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[BETA_EFF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_beta_eff_d_temp[_qp][i] =
        (*_spline_interps[BETA_EFF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[BETA_EFF])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] =
        (*_spline_interps[DECAY_CONSTANT])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[DECAY_CONSTANT])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_decay_constant_d_temp[_qp][i] =
        (*_spline_interps[DECAY_CONSTANT])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_spline_interps[DECAY_CONSTANT])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
  }
}

void
DiffusionRodMaterial::monotoneCubicComputeQpProperties()
{
  Real vol_frac = volumeFraction();
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] =
        (*_monotone_cubic_interps[REMXS])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[REMXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _fissxs[_qp][i] =
        (*_monotone_cubic_interps[FISSXS])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[FISSXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _nsf[_qp][i] =
        (*_monotone_cubic_interps[NSF])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[NSF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _fisse[_qp][i] =
        ((*_monotone_cubic_interps[FISSE])[i].sample(_temperature[_qp]) * vol_frac +
         (*_nonrod_monotone_cubic_interps[FISSE])[i].sample(_temperature[_qp]) * (1 - vol_frac)) *
        1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] =
        (*_monotone_cubic_interps[DIFFCOEF])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[DIFFCOEF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _recipvel[_qp][i] =
        (*_monotone_cubic_interps[RECIPVEL])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[RECIPVEL])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_t[_qp][i] =
        (*_monotone_cubic_interps[CHI_T])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[CHI_T])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_p[_qp][i] =
        (*_monotone_cubic_interps[CHI_P])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[CHI_P])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_d[_qp][i] =
        (*_monotone_cubic_interps[CHI_D])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[CHI_D])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_remxs_d_temp[_qp][i] =
        (*_monotone_cubic_interps[REMXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[REMXS])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
    _d_fissxs_d_temp[_qp][i] =
        (*_monotone_cubic_interps[FISSXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[FISSXS])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
    _d_nsf_d_temp[_qp][i] =
        (*_monotone_cubic_interps[NSF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[NSF])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
    _d_fisse_d_temp[_qp][i] =
        ((*_monotone_cubic_interps[FISSE])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
         (*_nonrod_monotone_cubic_interps[FISSE])[i].sampleDerivative(_temperature[_qp]) *
             (1 - vol_frac)) *
        1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] =
        (*_monotone_cubic_interps[DIFFCOEF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[DIFFCOEF])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
    _d_recipvel_d_temp[_qp][i] =
        (*_monotone_cubic_interps[RECIPVEL])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[RECIPVEL])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
    _d_chi_t_d_temp[_qp][i] =
        (*_monotone_cubic_interps[CHI_T])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[CHI_T])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
    _d_chi_p_d_temp[_qp][i] =
        (*_monotone_cubic_interps[CHI_P])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[CHI_P])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
    _d_chi_d_d_temp[_qp][i] =
        (*_monotone_cubic_interps[CHI_D])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[CHI_D])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] =
        (*_monotone_cubic_interps[GTRANSFXS])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[GTRANSFXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_gtransfxs_d_temp[_qp][i] =
        (*_monotone_cubic_interps[GTRANSFXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[GTRANSFXS])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] =
        (*_monotone_cubic_interps[BETA_EFF])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[BETA_EFF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_beta_eff_d_temp[_qp][i] =
        (*_monotone_cubic_interps[BETA_EFF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[BETA_EFF])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] =
        (*_monotone_cubic_interps[DECAY_CONSTANT])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_monotone_cubic_interps[DECAY_CONSTANT])[i].sample(_temperature[_qp]) *
            (1 - vol_frac);
    _d_decay_constant_d_temp[_qp][i] =
        (*_monotone_cubic_interps[DECAY_CONSTANT])[i].sampleDerivative(_temperature[_qp]) *
            vol_frac +
        (*_nonrod_monotone_cubic_interps[DECAY_CONSTANT])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
  }
}

void
DiffusionRodMaterial::linearComputeQpProperties()
{
  Real vol_frac = volumeFraction();
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = (*_linear_interps[REMXS])[i].sample(_temperature[_qp]) * vol_frac +
                     (*_nonrod_linear_interps[REMXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _fissxs[_qp][i] =
        (*_linear_interps[FISSXS])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[FISSXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _nsf[_qp][i] = (*_linear_interps[NSF])[i].sample(_temperature[_qp]) * vol_frac +
                   (*_nonrod_linear_interps[NSF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _fisse[_qp][i] =
        ((*_linear_interps[FISSE])[i].sample(_temperature[_qp]) * vol_frac +
         (*_nonrod_linear_interps[FISSE])[i].sample(_temperature[_qp]) * (1 - vol_frac)) *
        1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] =
        (*_linear_interps[DIFFCOEF])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[DIFFCOEF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _recipvel[_qp][i] =
        (*_linear_interps[RECIPVEL])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[RECIPVEL])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_t[_qp][i] = (*_linear_interps[CHI_T])[i].sample(_temperature[_qp]) * vol_frac +
                     (*_nonrod_linear_interps[CHI_T])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_p[_qp][i] = (*_linear_interps[CHI_P])[i].sample(_temperature[_qp]) * vol_frac +
                     (*_nonrod_linear_interps[CHI_P])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _chi_d[_qp][i] = (*_linear_interps[CHI_D])[i].sample(_temperature[_qp]) * vol_frac +
                     (*_nonrod_linear_interps[CHI_D])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_remxs_d_temp[_qp][i] =
        (*_linear_interps[REMXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[REMXS])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_fissxs_d_temp[_qp][i] =
        (*_linear_interps[FISSXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[FISSXS])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_nsf_d_temp[_qp][i] =
        (*_linear_interps[NSF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[NSF])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_fisse_d_temp[_qp][i] =
        ((*_linear_interps[FISSE])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
         (*_nonrod_linear_interps[FISSE])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac)) *
        1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] =
        (*_linear_interps[DIFFCOEF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[DIFFCOEF])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_recipvel_d_temp[_qp][i] =
        (*_linear_interps[RECIPVEL])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[RECIPVEL])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_chi_t_d_temp[_qp][i] =
        (*_linear_interps[CHI_T])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[CHI_T])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_chi_p_d_temp[_qp][i] =
        (*_linear_interps[CHI_P])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[CHI_P])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _d_chi_d_d_temp[_qp][i] =
        (*_linear_interps[CHI_D])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[CHI_D])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] =
        (*_linear_interps[GTRANSFXS])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[GTRANSFXS])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_gtransfxs_d_temp[_qp][i] =
        (*_linear_interps[GTRANSFXS])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[GTRANSFXS])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] =
        (*_linear_interps[BETA_EFF])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[BETA_EFF])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_beta_eff_d_temp[_qp][i] =
        (*_linear_interps[BETA_EFF])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[BETA_EFF])[i].sampleDerivative(_temperature[_qp]) * (1 - vol_frac);
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] =
        (*_linear_interps[DECAY_CONSTANT])[i].sample(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[DECAY_CONSTANT])[i].sample(_temperature[_qp]) * (1 - vol_frac);
    _d_decay_constant_d_temp[_qp][i] =
        (*_linear_interps[DECAY_CONSTANT])[i].sampleDerivative(_temperature[_qp]) * vol_frac +
        (*_nonrod_linear_interps[DECAY_CONSTANT])[i].sampleDerivative(_temperature[_qp]) *
            (1 - vol_frac);
  }
}

Real
DiffusionRodMaterial::volumeFraction()
{
  const unsigned int num_nodes = _current_elem->n_nodes();
  const Node * const * elem_nodes = _current_elem->get_nodes();
  const Real rod_height = _rod_height.value(_t, _q_point[_qp]);
  Real elem_z_max = (*elem_nodes[0])(2);
  Real elem_z_min = (*elem_nodes[0])(2);
  for (unsigned int i = 1; i < num_nodes; ++i)
  {
    if ((*elem_nodes[i])(2) > elem_z_max)
      elem_z_max = (*elem_nodes[i])(2);
    else if ((*elem_nodes[i])(2) < elem_z_min)
      elem_z_min = (*elem_nodes[i])(2);
  }
  if (rod_height < elem_z_min)
    return 1.0;
  else if (rod_height > elem_z_max)
    return 0.0;
  Real corrected_vol_frac =
    std::min(
      std::pow((elem_z_max - rod_height) / (elem_z_max - elem_z_min), _cusp_power) * _cusp_coeff,
      1.0);
  return corrected_vol_frac;
}
