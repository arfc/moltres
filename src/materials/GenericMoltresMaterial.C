#include "GenericMoltresMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

template<>
InputParameters validParams<GenericMoltresMaterial>()
{
  InputParameters params = validParams<GenericConstantMaterial>();
  params.addRequiredParam<std::string>("property_tables_root", "The file root name containing interpolation tables for material properties.");
  params.addRequiredParam<int>("num_groups", "The number of groups the energy spectrum is divided into.");
  params.addCoupledVar("temperature", 937, "The temperature field for determining group constants.");
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
    _d_remxs_d_temp(declareProperty<std::vector<Real> >("d_remxs_d_temp")),
    _d_fissxs_d_temp(declareProperty<std::vector<Real> >("d_fissxs_d_temp")),
    _d_nsf_d_temp(declareProperty<std::vector<Real> >("d_nsf_d_temp")),
    _d_fisse_d_temp(declareProperty<std::vector<Real> >("d_fisse_d_temp")),
    _d_diffcoef_d_temp(declareProperty<std::vector<Real> >("d_diffcoef_d_temp")),
    _d_recipvel_d_temp(declareProperty<std::vector<Real> >("d_recipvel_d_temp")),
    _d_chi_d_temp(declareProperty<std::vector<Real> >("d_chi_d_temp")),
    _d_gtransfxs_d_temp(declareProperty<std::vector<Real> >("d_gtransfxs_d_temp"))

{
  int i, j, k;
  Real value;

  _num_groups = getParam<int>("num_groups");
  std::string property_tables_root = getParam<std::string>("property_tables_root");
  std::vector<std::string> xsec_names {"FLUX", "REMXS", "FISSXS", "NUBAR", "NSF", "FISSE", "DIFFCOEF", "RECIPVEL", "CHI", "GTRANSFXS"};

  std::vector<Real> temperature;
  std::string file_name = property_tables_root + "DIFFCOEF.txt";
  const std::string & file_name_ref = file_name;
  std::ifstream myfile (file_name_ref.c_str());
  if (myfile.is_open())
  {
    while (myfile >> value)
    {
      temperature.push_back(value);
      for (k = 0; k < _num_groups; ++k)
        myfile >> value;
    }
    myfile.close();
  }
  else
    mooseError("Unable to open file " << file_name);

  std::map<std::string, std::vector<std::vector<Real> > > xsec_map;
  for (j = 0; j < xsec_names.size(); ++j)
  {
    std::string file_name = property_tables_root + xsec_names[j] + ".txt";
    const std::string & file_name_ref = file_name;
    std::ifstream myfile (file_name_ref.c_str());
    if (xsec_names[j].compare("GTRANSFXS") == 0)
      _vec_lengths[xsec_names[j]] = _num_groups * _num_groups;
    else
      _vec_lengths[xsec_names[j]] = _num_groups;
    xsec_map[xsec_names[j]].resize(_vec_lengths[xsec_names[j]]);
    _xsec_interpolators[xsec_names[j]].resize(_vec_lengths[xsec_names[j]]);
    if (myfile.is_open())
    {
      while (myfile >> value)
      {
        for (k = 0; k < _vec_lengths[xsec_names[j]]; ++k)
        {
          myfile >> value;
          xsec_map[xsec_names[j]][k].push_back(value);
        }
      }
      myfile.close();
      for (k = 0; k < _vec_lengths[xsec_names[j]]; ++k)
        _xsec_interpolators[xsec_names[j]][k].setData(temperature, xsec_map[xsec_names[j]][k]);
    }
    else
      mooseError("Unable to open file " << file_name);
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
  _d_remxs_d_temp[_qp].resize(_vec_lengths["REMXS"]);
  _d_fissxs_d_temp[_qp].resize(_vec_lengths["FISSXS"]);
  _d_nsf_d_temp[_qp].resize(_vec_lengths["NSF"]);
  _d_fisse_d_temp[_qp].resize(_vec_lengths["FISSE"]);
  _d_diffcoef_d_temp[_qp].resize(_vec_lengths["DIFFCOEF"]);
  _d_recipvel_d_temp[_qp].resize(_vec_lengths["RECIPVEL"]);
  _d_chi_d_temp[_qp].resize(_vec_lengths["CHI"]);
  _d_gtransfxs_d_temp[_qp].resize(_vec_lengths["GTRANSFXS"]);

  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _xsec_interpolators["REMXS"][i].sample(_temperature[_qp]);
    _fissxs[_qp][i] = _xsec_interpolators["FISSXS"][i].sample(_temperature[_qp]);
    _nsf[_qp][i] = _xsec_interpolators["NSF"][i].sample(_temperature[_qp]);
    _fisse[_qp][i] = _xsec_interpolators["FISSE"][i].sample(_temperature[_qp]);
    _diffcoef[_qp][i] = _xsec_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
    _recipvel[_qp][i] = _xsec_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
    _chi[_qp][i] = _xsec_interpolators["CHI"][i].sample(_temperature[_qp]);
    _d_remxs_d_temp[_qp][i] = _xsec_interpolators["REMXS"][i].sampleDerivative(_temperature[_qp]);
    _d_fissxs_d_temp[_qp][i] = _xsec_interpolators["FISSXS"][i].sampleDerivative(_temperature[_qp]);
    _d_nsf_d_temp[_qp][i] = _xsec_interpolators["NSF"][i].sampleDerivative(_temperature[_qp]);
    _d_fisse_d_temp[_qp][i] = _xsec_interpolators["FISSE"][i].sampleDerivative(_temperature[_qp]);
    _d_diffcoef_d_temp[_qp][i] = _xsec_interpolators["DIFFCOEF"][i].sampleDerivative(_temperature[_qp]);
    _d_recipvel_d_temp[_qp][i] = _xsec_interpolators["RECIPVEL"][i].sampleDerivative(_temperature[_qp]);
    _d_chi_d_temp[_qp][i] = _xsec_interpolators["CHI"][i].sampleDerivative(_temperature[_qp]);
  }
  for (unsigned int i = 0; i < _num_groups * _num_groups; ++i)
  {
    _gtransfxs[_qp][i] = _xsec_interpolators["GTRANSFXS"][i].sample(_temperature[_qp]);
    _d_gtransfxs_d_temp[_qp][i] = _xsec_interpolators["GTRANSFXS"][i].sampleDerivative(_temperature[_qp]);
  }

}
