#include "MoltresJsonMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

registerMooseObject("MoltresApp", MoltresJsonMaterial);

InputParameters
MoltresJsonMaterial::validParams()
{
  InputParameters params = NuclearMaterial::validParams();
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
  //for (moosecontrib::Json::ValueIterator itr = temp_root.begin(); itr != temp_root.end(); itr++)
  {
    _XsTemperature[k] = el.value().get<int>();
    k = k + 1;
  }

  Construct(xs_root, _xsec_names);
}

void
MoltresJsonMaterial::Construct(nlohmann::json xs_root,
                               std::vector<std::string> _xsec_names)
{
  auto xsec_interpolators = _xsec_linear_interpolators;
  bool oneInfo = false;
  for (unsigned int j = 0; j < _xsec_names.size(); ++j)
  {

    auto o = _vec_lengths[_xsec_names[j]];
    auto L = _XsTemperature.size();

    _xsec_linear_interpolators[_xsec_names[j]].resize(o);
    _xsec_spline_interpolators[_xsec_names[j]].resize(o);
    _xsec_monotone_cubic_interpolators[_xsec_names[j]].resize(o);

    _xsec_map[_xsec_names[j]].resize(o);

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
                   _file_map[_xsec_names[j]]);

      int dims = dataset.size();
      if (o == 0 and !oneInfo)
      {
        mooseInfo("Only precursor material data initialized (num_groups = 0) for material " + _name);
        oneInfo = true;
      }
      if (o != dims && o != 0)
        mooseError("The number of " + _material_key + "/" + temp_key + "/" +
                   _file_map[_xsec_names[j]] + " values does not match the "
                   "num_groups/num_precursor_groups parameter. " +
                   std::to_string(dims) + "!=" + std::to_string(o));
      for (auto k = 0; k < o; ++k)
      {
        _xsec_map[_xsec_names[j]][k].push_back(dataset[k].get<double>());
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
        break;
      case LINEAR:
        for (auto k = 0; k < o; ++k)
        {
          _xsec_linear_interpolators[_xsec_names[j]][k].setData(_XsTemperature,
                                                                _xsec_map[_xsec_names[j]][k]);
        }
        break;
      case SPLINE:
        for (auto k = 0; k < o; ++k)
        {
          _xsec_spline_interpolators[_xsec_names[j]][k].setData(_XsTemperature,
                                                                _xsec_map[_xsec_names[j]][k]);
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
        }
        break;
      default:
        mooseError("Invalid enum type for interp_type");
        break;
    }
  }
}

void
MoltresJsonMaterial::computeQpProperties()
{
  NuclearMaterial::preComputeQpProperties();

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
      splineComputeQpProperties();
      break;
    case MONOTONE_CUBIC:
      monotoneCubicComputeQpProperties();
      break;
    default:
      mooseError("Invalid enum type for interp_type");
      break;
  }
}
