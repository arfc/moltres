#include "DiffusionRodMaterial.h"
#include "Function.h"
#include "MooseUtils.h"

registerMooseObject("MoltresApp", DiffusionRodMaterial);

InputParameters
DiffusionRodMaterial::validParams()
{
  InputParameters params = MoltresJsonMaterial::validParams();
  params.addRequiredParam<std::string>("nonrod_material_key",
                                       "The material key for the non-rod material.");
  params.addRequiredParam<FunctionName>("rod_height_func",
      "Name of function that provides the rod interface height");
  return params;
}

DiffusionRodMaterial::DiffusionRodMaterial(const InputParameters & parameters)
  : MoltresJsonMaterial(parameters),
    _nonrod_material_key(getParam<std::string>("nonrod_material_key")),
    _rod_height(getFunction("rod_height_func"))
{
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
  std::string nr = "nonrod_";
  Real volFrac = volumeFraction();
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] =
      _xsec_map["REMXS"][i][0] * volFrac + _xsec_map[nr + "REMXS"][i][0] * (1 - volFrac);
    _fissxs[_qp][i] =
      _xsec_map["FISSXS"][i][0] * volFrac + _xsec_map[nr + "FISSXS"][i][0] * (1 - volFrac);
    _nsf[_qp][i] =
      _xsec_map["NSF"][i][0] * volFrac + _xsec_map[nr + "NSF"][i][0] * (1 - volFrac);
    _fisse[_qp][i] =
      (_xsec_map["FISSE"][i][0] * volFrac + _xsec_map[nr + "FISSE"][i][0] * (1 - volFrac)) *
      1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] =
      _xsec_map["DIFFCOEF"][i][0] * volFrac + _xsec_map[nr + "DIFFCOEF"][i][0] * (1 - volFrac);
    _recipvel[_qp][i] =
      _xsec_map["RECIPVEL"][i][0] * volFrac + _xsec_map[nr + "RECIPVEL"][i][0] * (1 - volFrac);
    _chi_t[_qp][i] =
      _xsec_map["CHI_T"][i][0] * volFrac + _xsec_map[nr + "CHI_T"][i][0] * (1 - volFrac);
    _chi_p[_qp][i] =
      _xsec_map["CHI_P"][i][0] * volFrac + _xsec_map[nr + "CHI_P"][i][0] * (1 - volFrac);
    _chi_d[_qp][i] =
      _xsec_map["CHI_D"][i][0] * volFrac + _xsec_map[nr + "CHI_D"][i][0] * (1 - volFrac);
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
    _gtransfxs[_qp][i] =
      _xsec_map["GTRANSFXS"][i][0] * volFrac + _xsec_map[nr + "GTRANSFXS"][i][0] * (1 - volFrac);
    _d_gtransfxs_d_temp[_qp][i] = 0;
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] =
      _xsec_map["BETA_EFF"][i][0] * volFrac + _xsec_map[nr + "BETA_EFF"][i][0] * (1 - volFrac);
    _d_beta_eff_d_temp[_qp][i] = 0;
    _beta[_qp] += _beta_eff[_qp][i];
    _decay_constant[_qp][i] =
      _xsec_map["DECAY_CONSTANT"][i][0] * volFrac +
      _xsec_map[nr + "DECAY_CONSTANT"][i][0] * (1 - volFrac);
    _d_decay_constant_d_temp[_qp][i] = 0;
  }
}

void
DiffusionRodMaterial::splineComputeQpProperties()
{
  std::string nr = "nonrod_";
  Real volFrac = volumeFraction();
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] =
      _xsec_spline_interpolators["REMXS"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "REMXS"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _fissxs[_qp][i] =
      _xsec_spline_interpolators["FISSXS"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "FISSXS"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _nsf[_qp][i] =
      _xsec_spline_interpolators["NSF"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "NSF"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _fisse[_qp][i] =
      (_xsec_spline_interpolators["FISSE"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "FISSE"][i].sample(_temperature[_qp]) * (1 - volFrac))
      * 1e6 * 1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] =
      _xsec_spline_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "DIFFCOEF"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _recipvel[_qp][i] =
      _xsec_spline_interpolators["RECIPVEL"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "RECIPVEL"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _chi_t[_qp][i] =
      _xsec_spline_interpolators["CHI_T"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "CHI_T"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _chi_p[_qp][i] =
      _xsec_spline_interpolators["CHI_P"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "CHI_P"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _chi_d[_qp][i] =
      _xsec_spline_interpolators["CHI_D"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "CHI_D"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _d_remxs_d_temp[_qp][i] =
      _xsec_spline_interpolators["REMXS"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "REMXS"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
    _d_fissxs_d_temp[_qp][i] =
      _xsec_spline_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "FISSXS"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
    _d_nsf_d_temp[_qp][i] =
        _xsec_spline_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "NSF"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
    _d_fisse_d_temp[_qp][i] =
      (_xsec_spline_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "FISSE"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac)) * 1e6 * 1.6e-19; // convert from MeV to Joules
    _d_diffcoef_d_temp[_qp][i] =
      _xsec_spline_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "DIFFCOEF"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
    _d_recipvel_d_temp[_qp][i] =
      _xsec_spline_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "RECIPVEL"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
    _d_chi_t_d_temp[_qp][i] =
      _xsec_spline_interpolators["CHI_T"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "CHI_T"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
    _d_chi_p_d_temp[_qp][i] =
      _xsec_spline_interpolators["CHI_P"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "CHI_P"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
    _d_chi_d_d_temp[_qp][i] =
      _xsec_spline_interpolators["CHI_D"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "CHI_D"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
  }
  for (decltype(_num_groups) i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] =
      _xsec_spline_interpolators["GTRANSFXS"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "GTRANSFXS"][i].sample(_temperature[_qp]) *
      (1 - volFrac);
    _d_gtransfxs_d_temp[_qp][i] =
      _xsec_spline_interpolators["GTRANSFXS"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "GTRANSFXS"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
  }
  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (decltype(_num_groups) i = 0; i < _num_precursor_groups; ++i)
  {
    _beta_eff[_qp][i] =
      _xsec_spline_interpolators["BETA_EFF"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "BETA_EFF"][i].sample(_temperature[_qp]) * (1 - volFrac);
    _d_beta_eff_d_temp[_qp][i] =
      _xsec_spline_interpolators["BETA_EFF"][i].sampleDerivative(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "BETA_EFF"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
    _beta[_qp] += _beta_eff[_qp][i];
    _d_beta_d_temp[_qp] += _d_beta_eff_d_temp[_qp][i];
    _decay_constant[_qp][i] =
      _xsec_spline_interpolators["DECAY_CONSTANT"][i].sample(_temperature[_qp]) * volFrac +
      _xsec_spline_interpolators[nr + "DECAY_CONSTANT"][i].sample(_temperature[_qp]) *
      (1 - volFrac);
    _d_decay_constant_d_temp[_qp][i] =
      _xsec_spline_interpolators["DECAY_CONSTANT"][i].sampleDerivative(_temperature[_qp]) * volFrac
      + _xsec_spline_interpolators[nr + "DECAY_CONSTANT"][i].sampleDerivative(_temperature[_qp]) *
      (1 - volFrac);
  }
}

void
DiffusionRodMaterial::monotoneCubicComputeQpProperties()
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
DiffusionRodMaterial::linearComputeQpProperties()
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
    if ((*elem_nodes[i])(2) < elem_z_min)
      elem_z_min = (*elem_nodes[i])(2);
  }

  if (rod_height < elem_z_min)
    return 1.0;
  else if (rod_height > elem_z_max)
    return 0.0;
  else
    return (elem_z_max - rod_height) / (elem_z_max - elem_z_min);
}
