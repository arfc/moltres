#include "NukeMaterial.h"
#include "MooseUtils.h"
#define PRINT(var) #var

template<>
InputParameters validParams<NukeMaterial>()
{
  InputParameters params = validParams<Material>();
  params.addRequiredParam<std::string>("property_tables_root", "The file root name containing interpolation tables for material properties.");
  params.addRequiredParam<int>("num_groups", "The number of groups the energy spectrum is divided into.");
  return params;
}


NukeMaterial::NukeMaterial(const InputParameters & parameters) :
    Material(parameters),
{
  std::vector<std::vector<float> > flux;
  std::vector<std::vector<float> > remxs;
  std::vector<std::vector<float> > fissxs;
  std::vector<std::vector<float> > nubar;
  std::vector<std::vector<float> > nsf;
  std::vector<std::vector<float> > fisse;
  std::vector<std::vector<float> > diffcoef;
  std::vector<std::vector<float> > chi;
  std::vector<std::vector<float> > gtransfxs;

  int _num_groups = 2;
  std::string property_tables_root = "some_name";
  std::vector<std::vector<std::vector<Real> > > xsec_vars {flux, remxs, fissxs, nubar, nsf, fisse, diffcoef, chi, gtransfxs};
  std::vector<int> xsec_column_lengths;
  for (i=0; i<= {_num_groups, _num_groups, _num_groups, _num_groups,
  for (const auto &i : xsec_vars)
    for (const auto &j : i)
      for (const auto &k : j)
        std::cout << k << std::endl;
  const char *charPath = property_tables_root.c_str();
  std::ifstream myfile (charPath);
  Real value;

  if (myfile.is_open())
  {
    while ( myfile >> value )
    {
      actual_mean_energy.push_back(value);
      myfile >> value;
      alpha.push_back(value);
      myfile >> value;
      alphaEx.push_back(value);
      myfile >> value;
      alphaEl.push_back(value);
      myfile >> value;
      mu.push_back(value);
      myfile >> value;
      diff.push_back(value);
    }
    myfile.close();
  }

  else std::cerr << "Unable to open file" << std::endl;

  _alpha_interpolation.setData(actual_mean_energy, alpha);
  _alphaEx_interpolation.setData(actual_mean_energy, alphaEx);
  _alphaEl_interpolation.setData(actual_mean_energy, alphaEl);
  _mu_interpolation.setData(actual_mean_energy, mu);
  _diff_interpolation.setData(actual_mean_energy, diff);
}

void
NukeMaterial::computeQpProperties()
{
  if (_interp_trans_coeffs)
  {
    if (_ramp_trans_coeffs)
    {
      _muem[_qp] = (std::tanh(_t / 1e-6) * _mu_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) + (1. - std::tanh(_t / 1e-6)) * .0352) * _voltage_scaling;
      _d_muem_d_actual_mean_en[_qp] = std::tanh(_t / 1e-6) * _mu_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
      _diffem[_qp] = std::tanh(_t / 1e-6) * _diff_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) + (1. - std::tanh(_t / 1e-6)) * .30;
      _d_diffem_d_actual_mean_en[_qp] = std::tanh(_t / 1e-6) * _diff_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
    }
    else
    {
      _muem[_qp] = _mu_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
      _d_muem_d_actual_mean_en[_qp] = _mu_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp])) * _voltage_scaling;
      _diffem[_qp] = _diff_interpolation.sample(std::exp(_mean_en[_qp]-_em[_qp]));
      _d_diffem_d_actual_mean_en[_qp] = _diff_interpolation.sampleDerivative(std::exp(_mean_en[_qp]-_em[_qp]));
    }
  }
  else
  {
    // From bolos at atmospheric pressure and an EField of 2e5 V/m
    _muem[_qp] = 0.0352103411399 * _voltage_scaling; // units of m^2/(kV*s) if _voltage_scaling = 1000
    _d_muem_d_actual_mean_en[_qp] = 0.0;
    _diffem[_qp] = 0.297951680159;
    _d_diffem_d_actual_mean_en[_qp] = 0.0;
  }
}
