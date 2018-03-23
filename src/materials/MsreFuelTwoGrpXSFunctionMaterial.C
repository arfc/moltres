#include "MsreFuelTwoGrpXSFunctionMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

registerMooseObject("MoltresApp", MsreFuelTwoGrpXSFunctionMaterial);

template <>
InputParameters
validParams<MsreFuelTwoGrpXSFunctionMaterial>()
{
  InputParameters params = validParams<GenericConstantMaterial>();
  params.addCoupledVar(
      "temperature", 937, "The temperature field for determining group constants.");
  return params;
}

MsreFuelTwoGrpXSFunctionMaterial::MsreFuelTwoGrpXSFunctionMaterial(
    const InputParameters & parameters)
  : GenericConstantMaterial(parameters),
    _T(coupledValue("temperature")),
    // _rho(getMaterialProperty<Real>("rho")),
    _remxs(declareProperty<std::vector<Real>>("remxs")),
    _fissxs(declareProperty<std::vector<Real>>("fissxs")),
    _nsf(declareProperty<std::vector<Real>>("nsf")),
    _fisse(declareProperty<std::vector<Real>>("fisse")),
    _diffcoef(declareProperty<std::vector<Real>>("diffcoef")),
    _recipvel(declareProperty<std::vector<Real>>("recipvel")),
    _chi(declareProperty<std::vector<Real>>("chi")),
    _gtransfxs(declareProperty<std::vector<Real>>("gtransfxs")),
    _beta_eff(declareProperty<std::vector<Real>>("beta_eff")),
    _beta(declareProperty<Real>("beta")),
    _decay_constant(declareProperty<std::vector<Real>>("decay_constant")),

    _d_remxs_d_temp(declareProperty<std::vector<Real>>("d_remxs_d_temp")),
    _d_fissxs_d_temp(declareProperty<std::vector<Real>>("d_fissxs_d_temp")),
    _d_nsf_d_temp(declareProperty<std::vector<Real>>("d_nsf_d_temp")),
    _d_fisse_d_temp(declareProperty<std::vector<Real>>("d_fisse_d_temp")),
    _d_diffcoef_d_temp(declareProperty<std::vector<Real>>("d_diffcoef_d_temp")),
    _d_recipvel_d_temp(declareProperty<std::vector<Real>>("d_recipvel_d_temp")),
    _d_chi_d_temp(declareProperty<std::vector<Real>>("d_chi_d_temp")),
    _d_gtransfxs_d_temp(declareProperty<std::vector<Real>>("d_gtransfxs_d_temp")),
    _d_beta_eff_d_temp(declareProperty<std::vector<Real>>("d_beta_eff_d_temp")),
    _d_beta_d_temp(declareProperty<Real>("d_beta_d_temp")),
    _d_decay_constant_d_temp(declareProperty<std::vector<Real>>("d_decay_constant_d_temp"))
{
}

void
MsreFuelTwoGrpXSFunctionMaterial::computeQpProperties()
{
  for (unsigned int i = 0; i < _num_props; i++)
    (*_properties[i])[_qp] = _prop_values[i];

  _remxs[_qp].resize(2, 0);
  _fissxs[_qp].resize(2, 0);
  _nsf[_qp].resize(2, 0);
  _fisse[_qp].resize(2, 0);
  _diffcoef[_qp].resize(2, 0);
  _recipvel[_qp].resize(2, 0);
  _chi[_qp].resize(2, 0);
  _gtransfxs[_qp].resize(4, 0);
  _beta_eff[_qp].resize(6, 0);
  _decay_constant[_qp].resize(6, 0);
  _d_remxs_d_temp[_qp].resize(2, 0);
  _d_fissxs_d_temp[_qp].resize(2, 0);
  _d_nsf_d_temp[_qp].resize(2, 0);
  _d_fisse_d_temp[_qp].resize(2, 0);
  _d_diffcoef_d_temp[_qp].resize(2, 0);
  _d_recipvel_d_temp[_qp].resize(2, 0);
  _d_chi_d_temp[_qp].resize(2, 0);
  _d_gtransfxs_d_temp[_qp].resize(4, 0);
  _d_beta_eff_d_temp[_qp].resize(6, 0);
  _d_decay_constant_d_temp[_qp].resize(6, 0);

  Real T0 = 922;
  Real rho0 = 2.146e-3;
  Real rho = rho0 * std::exp(-1.8 * 1.18e-4 * (_T[_qp] - T0));
  Real d_rho_d_T = rho * -1.8 * 1.18e-4;

  // Constants taken from newt_msre property text files
  _nsf[_qp][0] = (3.18E-03 + -3.46E-04 * std::log(_T[_qp] / T0)) * rho / rho0;
  _nsf[_qp][1] = (5.30E-02 + -2.97E-02 * std::log(_T[_qp] / T0)) * rho / rho0;
  _remxs[_qp][0] = (5.72E-03 + 1.15E-03 * std::log(_T[_qp] / T0)) * rho / rho0;
  _remxs[_qp][1] = (2.86E-02 + -1.09E-02 * std::log(_T[_qp] / T0)) * rho / rho0;
  _fissxs[_qp][0] = (1.30E-03 + -1.41E-04 * std::log(_T[_qp] / T0)) * rho / rho0;
  _fissxs[_qp][1] = (2.18E-02 + -1.22E-02 * std::log(_T[_qp] / T0)) * rho / rho0;
  _diffcoef[_qp][0] = (1.43E+00 + 2.91E-01 * std::log(_T[_qp] / T0)) * rho0 / rho;
  _diffcoef[_qp][1] = (1.26E+00 + 2.92E-01 * std::log(_T[_qp] / T0)) * rho0 / rho;
  _gtransfxs[_qp][0] = (2.66E-01 + -5.57E-02 * std::log(_T[_qp] / T0)) * rho / rho0;
  _gtransfxs[_qp][1] = (1.59E-03 + 3.90E-03 * std::log(_T[_qp] / T0)) * rho / rho0;
  _gtransfxs[_qp][2] = (2.02E-03 + 1.44E-03 * std::log(_T[_qp] / T0)) * rho / rho0;
  _gtransfxs[_qp][3] = (2.49E-01 + -5.23E-02 * std::log(_T[_qp] / T0)) * rho / rho0;
  _recipvel[_qp][0] = 9.69E-08 + 2.18E-08 * std::log(_T[_qp] / T0);
  _recipvel[_qp][1] = 2.06E-06 + -6.95E-07 * std::log(_T[_qp] / T0);

  _d_nsf_d_temp[_qp][0] =
      (-3.46E-04 / _T[_qp] * rho + (3.18E-03 + -3.46E-04 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_nsf_d_temp[_qp][1] =
      (-2.97E-02 / _T[_qp] * rho + (5.30E-02 + -2.97E-02 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_remxs_d_temp[_qp][0] =
      (1.15E-03 / _T[_qp] * rho + (5.72E-03 + 1.15E-03 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_remxs_d_temp[_qp][1] =
      (-1.09E-02 / _T[_qp] * rho + (2.86E-02 + -1.09E-02 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_fissxs_d_temp[_qp][0] =
      (-1.41E-04 / _T[_qp] * rho + (1.30E-03 + -1.41E-04 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_fissxs_d_temp[_qp][1] =
      (-1.22E-02 / _T[_qp] * rho + (2.18E-02 + -1.22E-02 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_diffcoef_d_temp[_qp][0] =
      (2.91E-01 / _T[_qp] / rho +
       (1.43E+00 + 2.91E-01 * std::log(_T[_qp] / T0)) * -d_rho_d_T / (rho * rho)) *
      rho0;
  _d_diffcoef_d_temp[_qp][1] =
      (2.92E-01 / _T[_qp] / rho +
       (1.26E+00 + 2.92E-01 * std::log(_T[_qp] / T0)) * -d_rho_d_T / (rho * rho)) *
      rho0;
  _d_gtransfxs_d_temp[_qp][0] =
      (-5.57E-02 / _T[_qp] * rho + (2.66E-01 + -5.57E-02 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_gtransfxs_d_temp[_qp][1] =
      (3.90E-03 / _T[_qp] * rho + (1.59E-03 + 3.90E-03 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_gtransfxs_d_temp[_qp][2] =
      (1.44E-03 / _T[_qp] * rho + (2.02E-03 + 1.44E-03 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_gtransfxs_d_temp[_qp][3] =
      (-5.23E-02 / _T[_qp] * rho + (2.49E-01 + -5.23E-02 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_recipvel_d_temp[_qp][0] = 2.18E-08 / _T[_qp];
  _d_recipvel_d_temp[_qp][1] = -6.95E-07 / _T[_qp];

  _fisse[_qp][0] = 194. * 1e6 * 1.6e-19; // convert MeV to Joules
  _fisse[_qp][1] = 194. * 1e6 * 1.6e-19; // convert MeV to Joules
  _chi[_qp][0] = 1;
  _chi[_qp][1] = 0;
  _beta_eff[_qp][0] = 2.37e-4;
  _beta_eff[_qp][1] = 1.54e-3;
  _beta_eff[_qp][2] = 1.38e-3;
  _beta_eff[_qp][3] = 2.80e-3;
  _beta_eff[_qp][4] = 8.26e-4;
  _beta_eff[_qp][5] = 2.86e-4;
  _beta[_qp] = 0;
  for (unsigned i = 0; i < 6; i++)
    _beta[_qp] += _beta_eff[_qp][i];
  _d_beta_d_temp[_qp] = 0;

  _decay_constant[_qp][0] = 1.24e-2;
  _decay_constant[_qp][1] = 3.06e-2;
  _decay_constant[_qp][2] = 1.11e-1;
  _decay_constant[_qp][3] = 3.02e-1;
  _decay_constant[_qp][4] = 1.17e0;
  _decay_constant[_qp][5] = 3.07e0;
}
