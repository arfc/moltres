#include "GraphiteTwoGrpXSFunctionMaterial.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

template <>
InputParameters
validParams<GraphiteTwoGrpXSFunctionMaterial>()
{
  InputParameters params = validParams<GenericConstantMaterial>();
  params.addCoupledVar(
      "temperature", 937, "The temperature field for determining group constants.");
  return params;
}

GraphiteTwoGrpXSFunctionMaterial::GraphiteTwoGrpXSFunctionMaterial(
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
GraphiteTwoGrpXSFunctionMaterial::computeQpProperties()
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
  Real rho0 = 1.86e-3;
  Real rho = rho0 * std::exp(-1.8 * 1.0e-5 * (_T[_qp] - T0));
  Real d_rho_d_T = rho * -1.8 * 1.0e-5;

  // Constants taken from newt_msre property text files
  _remxs[_qp][0] = (4.26E-03 + 3.78E-03 * std::log(_T[_qp] / T0)) * rho / rho0;
  _remxs[_qp][1] = (3.11E-03 + 8.38E-03 * std::log(_T[_qp] / T0)) * rho / rho0;
  _diffcoef[_qp][0] = (9.85E-01 + 8.23E-03 * std::log(_T[_qp] / T0)) * rho0 / rho;
  _diffcoef[_qp][1] = (7.70E-01 + 3.38E-03 * std::log(_T[_qp] / T0)) * rho0 / rho;
  _gtransfxs[_qp][0] = (3.94E-01 + -7.85E-03 * std::log(_T[_qp] / T0)) * rho / rho0;
  _gtransfxs[_qp][1] = (2.96E-03 + 8.43E-03 * std::log(_T[_qp] / T0)) * rho / rho0;
  _gtransfxs[_qp][2] = (4.25E-03 + 3.78E-03 * std::log(_T[_qp] / T0)) * rho / rho0;
  _gtransfxs[_qp][3] = (4.55E-01 + -1.05E-02 * std::log(_T[_qp] / T0)) * rho / rho0;
  _recipvel[_qp][0] = 9.98E-08 + 2.21E-08 * std::log(_T[_qp] / T0);
  _recipvel[_qp][1] = 2.08E-06 + -7.19E-07 * std::log(_T[_qp] / T0);

  _d_remxs_d_temp[_qp][0] =
      (3.78E-03 / _T[_qp] * rho + (4.26E-03 + 3.78E-03 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_remxs_d_temp[_qp][1] =
      (8.38E-03 / _T[_qp] * rho + (3.11E-03 + 8.38E-03 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_diffcoef_d_temp[_qp][0] =
      (8.23E-03 / _T[_qp] / rho +
       (9.85E-01 + 8.23E-03 * std::log(_T[_qp] / T0)) * -d_rho_d_T / (rho * rho)) *
      rho0;
  _d_diffcoef_d_temp[_qp][1] =
      (3.38E-03 / _T[_qp] / rho +
       (7.70E-01 + 3.38E-03 * std::log(_T[_qp] / T0)) * -d_rho_d_T / (rho * rho)) *
      rho0;
  _d_gtransfxs_d_temp[_qp][0] =
      (-7.85E-03 / _T[_qp] * rho + (3.94E-01 + -7.85E-03 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_gtransfxs_d_temp[_qp][1] =
      (8.43E-03 / _T[_qp] * rho + (2.96E-03 + 8.43E-03 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_gtransfxs_d_temp[_qp][2] =
      (3.78E-03 / _T[_qp] * rho + (4.25E-03 + 3.78E-03 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_gtransfxs_d_temp[_qp][3] =
      (-1.05E-02 / _T[_qp] * rho + (4.55E-01 + -1.05E-02 * std::log(_T[_qp] / T0)) * d_rho_d_T) /
      rho0;
  _d_recipvel_d_temp[_qp][0] = 2.21E-08 / _T[_qp];
  _d_recipvel_d_temp[_qp][1] = -7.19E-07 / _T[_qp];

  _beta[_qp] = 0;
  _d_beta_d_temp[_qp] = 0;
  for (unsigned i = 0; i < 2; i++)
  {
    _nsf[_qp][i] = 0;
    _fissxs[_qp][i] = 0;
    _d_nsf_d_temp[_qp][i] = 0;
    _d_fissxs_d_temp[_qp][i] = 0;
  }
  for (unsigned i = 0; i < 6; i++)
  {
    _beta_eff[_qp][i] = 0;
    _decay_constant[_qp][i] = 0;
    _d_beta_eff_d_temp[_qp][i] = 0;
    _d_decay_constant_d_temp[_qp][i] = 0;
  }
}
