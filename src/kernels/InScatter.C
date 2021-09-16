#include "InScatter.h"

registerMooseObject("MoltresApp", InScatter);

InputParameters
InScatter::validParams()
{
  InputParameters params = Kernel::validParams();
  params += ScalarTransportBase::validParams();
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. "
                                               "These MUST be listed by decreasing "
                                               "energy/increasing group number.");
  params.addParam<bool>(
      "sss2_input", true, "Whether serpent 2 was used to generate the input files.");
  return params;
}

InScatter::InScatter(const InputParameters & parameters)
  : Kernel(parameters),
    ScalarTransportBase(parameters),
    _gtransfxs(getMaterialProperty<std::vector<Real>>("gtransfxs")),
    _d_gtransfxs_d_temp(getMaterialProperty<std::vector<Real>>("d_gtransfxs_d_temp")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
    _temp_id(coupled("temperature")),
    _sss2_input(getParam<bool>("sss2_input"))
{
  unsigned int n = coupledComponents("group_fluxes");
  if (!(n == _num_groups))
  {
    mooseError("The number of coupled variables doesn't match the number of groups.");
  }
  _group_fluxes.resize(n);
  _flux_ids.resize(n);
  for (unsigned int i = 0; i < _group_fluxes.size(); ++i)
  {
    _group_fluxes[i] = &coupledValue("group_fluxes", i);
    _flux_ids[i] = coupled("group_fluxes", i);
  }
}

Real
InScatter::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (i == _group)
      continue;
    if (_sss2_input)
      r += -_test[_i][_qp] * _gtransfxs[_qp][i * _num_groups + _group] *
           computeConcentration((*_group_fluxes[i]), _qp);
    else
      r += -_test[_i][_qp] * _gtransfxs[_qp][i + _group * _num_groups] *
           computeConcentration((*_group_fluxes[i]), _qp);
  }

  return r;
}

Real
InScatter::computeQpJacobian()
{
  return 0.;
}

Real
InScatter::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i] && jvar != _group)
    {
      if (_sss2_input)
        jac += -_test[_i][_qp] * _gtransfxs[_qp][i * _num_groups + _group] *
               computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp);
      else
        jac += -_test[_i][_qp] * _gtransfxs[_qp][i + _group * _num_groups] *
               computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp);
      break;
    }
  }

  if (jvar == _temp_id)
  {
    for (unsigned int i = 0; i < _num_groups; ++i)
    {
      if (i == _group)
        continue;
      if (_sss2_input)
        jac += -_test[_i][_qp] * _d_gtransfxs_d_temp[_qp][i * _num_groups + _group] * _phi[_j][_qp] *
               computeConcentration((*_group_fluxes[i]), _qp);
      else
        jac += -_test[_i][_qp] * _d_gtransfxs_d_temp[_qp][i + _group * _num_groups] * _phi[_j][_qp] *
               computeConcentration((*_group_fluxes[i]), _qp);
    }
  }

  return jac;
}
