#include "DelayedNeutronSource.h"

registerMooseObject("MoltresApp", DelayedNeutronSource);

InputParameters
DelayedNeutronSource::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredParam<unsigned int>("num_precursor_groups", "The number of precursor groups.");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  params.addRequiredCoupledVar("pre_concs", "All the variables that hold the precursor "
                                            "concentrations. These MUST be listed by increasing "
                                            "group number.");
  params.addRequiredParam<unsigned int>("group_number","neutron energy group number for chi_d");
  params.addParam<Real>("eigenvalue_scaling", 1.0, "Eigenvalue scaling factor.");
  return params;
}

DelayedNeutronSource::DelayedNeutronSource(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _decay_constant(getMaterialProperty<std::vector<Real>>("decay_constant")),
    _d_decay_constant_d_temp(getMaterialProperty<std::vector<Real>>("d_decay_constant_d_temp")),
    _group(getParam<unsigned int>("group_number")),
    _chi_d(getMaterialProperty<std::vector<Real>>("chi_d")),
    _num_precursor_groups(getParam<unsigned int>("num_precursor_groups")),
    _temp_id(coupled("temperature")),
    _temp(coupledValue("temperature")),
    _eigenvalue_scaling(getParam<Real>("eigenvalue_scaling"))
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
DelayedNeutronSource::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_precursor_groups; ++i)
    r += -_decay_constant[_qp][i] * computeConcentration((*_pre_concs[i]), _qp);

  if (!(_eigenvalue_scaling == 1.0))
    r /= _eigenvalue_scaling;

  return _chi_d[_qp][_group-1] * _test[_i][_qp] * r;
}

Real
DelayedNeutronSource::computeQpJacobian()
{
  return 0.;
}

Real
DelayedNeutronSource::computeQpOffDiagJacobian(unsigned int jvar)
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

  if (!(_eigenvalue_scaling == 1.0))
    jac /= _eigenvalue_scaling;

  return _chi_d[_qp][_group-1] * jac;
}
