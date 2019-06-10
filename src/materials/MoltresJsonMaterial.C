#include "MoltresJsonMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

registerMooseObject("MoltresApp", MoltresJsonMaterial);

template <>
InputParameters
validParams<MoltresJsonMaterial>()
{
  InputParameters params = validParams<NuclearMaterial>();
  params.addRequiredParam<std::string>("base_file", "The file containing macroscopic XS.");
  params.addRequiredParam<std::string>("material_key",
                                       "The file key where the macroscopic XS can be found.");
  return params;
}

MoltresJsonMaterial::MoltresJsonMaterial(const InputParameters & parameters)
  : NuclearMaterial(parameters)

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

  const std::string & file_name_ref = base_file;
  std::ifstream myfile(file_name_ref.c_str());
  if (!myfile.good())
    mooseError("Unable to open XS file: " + base_file);
  moosecontrib::Json::Value xs_root;
  myfile >> xs_root;

  int k = 0;
  auto temp_root = xs_root[_material_key]["temp"];
  _XsTemperature.resize(temp_root.size());

  for (moosecontrib::Json::ValueIterator itr = temp_root.begin(); itr != temp_root.end(); itr++)
  {
    _XsTemperature[k] = itr->asInt();
    k = k + 1;
  }

  Construct(xs_root, xsec_names);
}

void
MoltresJsonMaterial::Construct(moosecontrib::Json::Value xs_root,
                               std::vector<std::string> xsec_names)
{
  auto xsec_interpolators = _xsec_linear_interpolators;
  for (decltype(xsec_names.size()) j = 0; j < xsec_names.size(); ++j)
  {

    auto o = _vec_lengths[xsec_names[j]];
    auto L = _XsTemperature.size();

    _xsec_linear_interpolators[xsec_names[j]].resize(o);
    _xsec_spline_interpolators[xsec_names[j]].resize(o);
    _xsec_monotone_cubic_interpolators[xsec_names[j]].resize(o);

    std::map<std::string, std::vector<std::vector<Real>>> xsec_map;
    xsec_map[xsec_names[j]].resize(o);

    for (decltype(_XsTemperature.size()) l = 0; l < L; ++l)
    {
      auto temp_key = std::to_string(static_cast<int>(_XsTemperature[l]));
      auto dataset = xs_root[_material_key][temp_key][xsec_names[j]];
      if (xsec_names[j] == "CHI_D" && dataset.empty())
      {
        for (decltype(_num_groups) k = 1; k < _num_groups; ++k)
          xsec_map["CHI_D"][k].push_back(0.0);
        xsec_map["CHI_D"][0].push_back(1.0);
        mooseWarning(
            "CHI_D data missing -> assume delayed neutrons born in top group for material " +
            _name);
        continue;
      }
      if (dataset.empty())
        mooseError("Unable to open database " + _material_key + "/" + temp_key + "/" +
                   _file_map[xsec_names[j]]);

      int dims = dataset.size();
      if (o != dims)
        mooseError("Dimensions of " + _material_key + "/" + temp_key + "/" +
                   _file_map[xsec_names[j]] + " and num_groups do not match\n" +
                   std::to_string(dims) + "!=" + std::to_string(o));
      for (decltype(_num_groups) k = 0; k < o; ++k)
      {
        xsec_map[xsec_names[j]][k].push_back(dataset[k].asDouble());
      }
    }
    switch (_interp_type)
    {
      case LSQ:
        mooseError("Least Squares not supported, please select \
          NONE, LINEAR, SPLICE, or MONOTONE_CUBIC ");
      case BICUBIC:
        mooseError("BICUBIC not supported, please select \
          NONE, LINEAR, SPLICE, or MONOTONE_CUBIC ");
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
      default:
        mooseError("Wrong enum type");
        break;
    }
  }
}

void
MoltresJsonMaterial::computeQpProperties()
{
  for (unsigned int i = 0; i < _num_props; i++)
    (*_properties[i])[_qp] = _prop_values[i];
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
    case LSQ:
      mooseError("Least Squares not supported, please select \
                  NONE, LINEAR, SPLICE, or MONOTONE_CUBIC ");
    case BICUBIC:
      mooseError("BICUBIC not supported, please select \
                  NONE, LINEAR, SPLICE, or MONOTONE_CUBIC ");
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
    default:
      mooseError("Wrong enum type");
      break;
  }
}
