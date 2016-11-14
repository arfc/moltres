#include "PrecursorSource.h"

template<>
InputParameters validParams<PrecursorSource>()
{
  InputParameters params = validParams<Kernel>();
  params += ScalarTransportBase<Kernel>::validParams();
  params.addRequiredParam<int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  params.addParam<int>("precursor_group_number", "What precursor group this kernel is acting on.");
  params.addCoupledVar("temperature", 937, "The temperature used to interpolate material properties.");
  return params;
}

PrecursorSource::PrecursorSource(const InputParameters & parameters) :
    ScalarTransportBase<Kernel>(parameters),
    _nsf(getMaterialProperty<std::vector<Real> >("nsf")),
    _d_nsf_d_temp(getMaterialProperty<std::vector<Real> >("d_nsf_d_temp")),
    _num_groups(getParam<int>("num_groups")),
    _beta_eff(getMaterialProperty<std::vector<Real> >("beta_eff")),
    _d_beta_eff_d_temp(getMaterialProperty<std::vector<Real> >("d_beta_eff_d_temp")),
    _precursor_group(getParam<int>("precursor_group_number") - 1),
    _temp(coupledValue("temperature")),
    _temp_id(coupled("temperature"))
{
  int n = coupledComponents("group_fluxes");
  if (!(n == _num_groups))
  {
    mooseError("The number of coupled variables doesn't match the number of groups.");
  }
  _group_fluxes.resize(n);
  _flux_ids.resize(n);
  for (int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _flux_ids[i] = coupled("group_fluxes", i);
  }
}

Real
PrecursorSource::computeQpResidual()
{
  Real r = 0;
  for (int i = 0; i < _num_groups; ++i)
  {
    r += -_test[_i][_qp] * _beta_eff[_qp][_precursor_group] * _nsf[_qp][i] * (*_group_fluxes[i])[_qp];
  }

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
  for (int i = 0; i < _num_groups; ++i)
    if (jvar == _flux_ids[i])
    {
      jac = -_test[_i][_qp] * _beta_eff[_qp][_precursor_group] * _nsf[_qp][i] * _phi[_j][_qp];
      return jac;
    }

  if (jvar == _temp_id)
  {
    for (int i = 0; i < _num_groups; ++i)
      jac += -_test[_i][_qp] * (_beta_eff[_qp][_precursor_group] * _d_nsf_d_temp[_qp][i] * _phi[_j][_qp] * (*_group_fluxes[i])[_qp] + _d_beta_eff_d_temp[_qp][_precursor_group] * _phi[_j][_qp] * _nsf[_qp][i] * (*_group_fluxes[i])[_qp]);
    return jac;
  }

  return 0;
}
