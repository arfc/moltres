#include "DelayedNeutronEigenSource.h"

registerMooseObject("MoltresApp", DelayedNeutronEigenSource);

template <>
InputParameters
validParams<DelayedNeutronEigenSource>()
{
  InputParameters params = validParams<EigenKernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<unsigned int>("num_precursor_groups", "The number of precursor groups.");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  params.addRequiredCoupledVar("pre_concs", "All the variables that hold the precursor "
                                            "concentrations. These MUST be listed by increasing "
                                            "group number.");
  params.addRequiredParam<unsigned int>("group_number","neutron energy group number for chi_d");
  return params;
}

DelayedNeutronEigenSource::DelayedNeutronEigenSource(const InputParameters & parameters)
  : EigenKernel(parameters),
    ScalarTransportBase(parameters),
    _decay_constant(getMaterialProperty<std::vector<Real>>("decay_constant")),
    _d_decay_constant_d_temp(getMaterialProperty<std::vector<Real>>("d_decay_constant_d_temp")),
    _group(getParam<unsigned int>("group_number")),
    _chi_d(getMaterialProperty<std::vector<Real>>("chi_d")),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _temp_id(coupled("temperature")),
    _temp(coupledValue("temperature"))
{
  unsigned int n = coupledComponents("pre_concs");
  if (!(n == _num_precursor_groups))
  {
    mooseError("The number of coupled variables doesn't match the number of groups.");
  }
  _pre_concs.resize(n);
  _pre_ids.resize(n);
  for (unsigned int i = 0; i < _pre_concs.size(); ++i)
  {
    _pre_concs[i] = &coupledValue("pre_concs", i);
    _pre_ids[i] = coupled("pre_concs", i);
  }
}

Real
DelayedNeutronEigenSource::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_precursor_groups; ++i)
    r += -_decay_constant[_qp][i] * computeConcentration((*_pre_concs[i]), _qp);

  return _chi_d[_qp][_group-1] * _test[_i][_qp] * r;
}

Real
DelayedNeutronEigenSource::computeQpJacobian()
{
  return 0.;
}

Real
DelayedNeutronEigenSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_precursor_groups; ++i)
  {
    if (jvar == _pre_ids[i])
    {
      jac += -_test[_i][_qp] * _decay_constant[_qp][i] *
             computeConcentrationDerivative((*_pre_concs[i]), _phi, _j, _qp);
      break;
    }
  }

  if (jvar == _temp_id)
    for (unsigned int i = 0; i < _num_precursor_groups; ++i)
      jac += -_test[_i][_qp] * computeConcentration((*_pre_concs[i]), _qp) *
             _d_decay_constant_d_temp[_qp][i] * _phi[_j][_qp];

  return _chi_d[_qp][_group-1] * jac;
}
