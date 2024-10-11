#include "PrecursorSource.h"

registerMooseObject("MoltresApp", PrecursorSource);

InputParameters
PrecursorSource::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. "
                                               "These MUST be listed by decreasing "
                                               "energy/increasing group number.");
  params.addParam<unsigned int>("precursor_group_number",
                                "What precursor group this kernel is acting on.");
  params.addCoupledVar(
      "temperature", 800, "The temperature used to interpolate material properties.");
  params.addParam<Real>("prec_scale", 1, "The factor by which the neutron fluxes are scaled.");
  params.addParam<PostprocessorName>("eigenvalue_scaling",
                        1.0,
                        "Artificial scaling factor for the fission source. Primarily for "
                        "introducing artificial reactivity to make super/subcritical systems "
                        "exactly critical or to simulate reactivity insertions/withdrawals.");
  return params;
}

PrecursorSource::PrecursorSource(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _nsf(getMaterialProperty<std::vector<Real>>("nsf")),
    _d_nsf_d_temp(getMaterialProperty<std::vector<Real>>("d_nsf_d_temp")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _beta_eff(getMaterialProperty<std::vector<Real>>("beta_eff")),
    _d_beta_eff_d_temp(getMaterialProperty<std::vector<Real>>("d_beta_eff_d_temp")),
    _precursor_group(getParam<unsigned int>("precursor_group_number") - 1),
    _temp(coupledValue("temperature")),
    _temp_id(coupled("temperature")),
    _prec_scale(getParam<Real>("prec_scale")),
    _eigenvalue_scaling(getPostprocessorValue("eigenvalue_scaling"))
{
  _group_fluxes.resize(_num_groups);
  _flux_ids.resize(_num_groups);
  for (unsigned int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _flux_ids[i] = coupled("group_fluxes", i);
  }
}

Real
PrecursorSource::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    r += -_test[_i][_qp] * _beta_eff[_qp][_precursor_group] * _nsf[_qp][i] *
         computeConcentration((*_group_fluxes[i]), _qp) * _prec_scale;
  }

  if ((_eigenvalue_scaling != 1.0))
    r /= _eigenvalue_scaling;

  return r;
}

Real
PrecursorSource::computeQpJacobian()
{
  return 0;
}

Real
PrecursorSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
    if (jvar == _flux_ids[i])
    {
      jac = -_test[_i][_qp] * _beta_eff[_qp][_precursor_group] * _nsf[_qp][i] *
            computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp) * _prec_scale;
      break;
    }

  if (jvar == _temp_id)
  {
    for (unsigned int i = 0; i < _num_groups; ++i)
      jac += -_test[_i][_qp] *
             (_beta_eff[_qp][_precursor_group] * _d_nsf_d_temp[_qp][i] * _phi[_j][_qp] *
                  computeConcentration((*_group_fluxes[i]), _qp) * _prec_scale +
              _d_beta_eff_d_temp[_qp][_precursor_group] * _phi[_j][_qp] * _nsf[_qp][i] *
                  computeConcentration((*_group_fluxes[i]), _qp) * _prec_scale);
  }

  if ((_eigenvalue_scaling != 1.0))
    jac /= _eigenvalue_scaling;

  return jac;
}
