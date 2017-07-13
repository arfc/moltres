#include "RoddedMaterial.h"
#include "MooseUtils.h"

template <>
InputParameters
validParams<RoddedMaterial>()
{
  InputParameters params = validParams<GenericMoltresMaterial>();
  params.addRequiredCoupledVar("rodPosition", "Position of the control rod.");
  params.addRequiredParam<Real>("absorb_factor",
                                "The material inherits from some other. How much more absorbing?");
  return params;
}

RoddedMaterial::RoddedMaterial(const InputParameters & parameters)
  : GenericMoltresMaterial(parameters),
    _absorb_factor(getParam<Real>("absorb_factor")),
    _rod_pos(coupledValue("rodPosition"))
{
  if (_interp_type != "spline")
    mooseError("Only spline works with this class. It's meant for testing.");
}

void
RoddedMaterial::computeSplineAbsorbingQpProperties()
{
  for (decltype(_num_groups) i = 0; i < _num_groups; ++i)
  {
    _remxs[_qp][i] = _xsec_spline_interpolators["REMXS"][i].sample(_temperature[_qp]) * _absorb_factor;
    _fissxs[_qp][i] = _xsec_spline_interpolators["FISSXS"][i].sample(_temperature[_qp]);
    _nsf[_qp][i] = _xsec_spline_interpolators["NSF"][i].sample(_temperature[_qp]);
    _fisse[_qp][i] = _xsec_spline_interpolators["FISSE"][i].sample(_temperature[_qp]) * 1e6 *
                     1.6e-19; // convert from MeV to Joules
    _diffcoef[_qp][i] = _xsec_spline_interpolators["DIFFCOEF"][i].sample(_temperature[_qp]);
    _recipvel[_qp][i] = _xsec_spline_interpolators["RECIPVEL"][i].sample(_temperature[_qp]);
    _chi[_qp][i] = _xsec_spline_interpolators["CHI"][i].sample(_temperature[_qp]);
    _d_remxs_d_temp[_qp][i] =
        _xsec_spline_interpolators["REMXS"][i].sampleDerivative(_temperature[_qp]) * _absorb_factor;
    _d_fissxs_d_temp[_qp][i] = 0.0;
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
RoddedMaterial::computeQpProperties()
{

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

  if (_q_point[_qp](2) < _rod_pos[_qp])
    GenericMoltresMaterial::computeQpProperties();
  else
    computeSplineAbsorbingQpProperties();
}
