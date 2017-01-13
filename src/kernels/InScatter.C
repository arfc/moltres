#include "InScatter.h"

template<>
InputParameters validParams<InScatter>()
{
  InputParameters params = validParams<Kernel>();
  params += validParams<ScalarTransportBase>();
  params.addRequiredParam<int>("group_number", "The current energy group");
  params.addRequiredParam<int>("num_groups", "The total numer of energy groups");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  return params;
}


InScatter::InScatter(const InputParameters & parameters) :
    Kernel(parameters),
    ScalarTransportBase(parameters),
    _gtransfxs(getMaterialProperty<std::vector<Real> >("gtransfxs")),
    _d_gtransfxs_d_temp(getMaterialProperty<std::vector<Real> >("d_gtransfxs_d_temp")),
    _group(getParam<int>("group_number") - 1),
    _num_groups(getParam<int>("num_groups")),
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
InScatter::computeQpResidual()
{
  Real r = 0;
  for (int i = 0; i < _num_groups; ++i)
  {
    if (i == _group)
      continue;
    r += -_test[_i][_qp] * _gtransfxs[_qp][i + _group * _num_groups] * computeConcentration((*_group_fluxes[i]), _qp);
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
  for (int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i]  && jvar != _group)
    {
      jac += -_test[_i][_qp] * _gtransfxs[_qp][i + _group * _num_groups] * computeConcentrationDerivative((*_group_fluxes[i]), _phi, _j, _qp);
      break;
    }
  }

  if (jvar == _temp_id)
  {
    for (int i = 0; i < _num_groups; ++i)
    {
      if (i == _group)
        continue;
      jac += -_test[_i][_qp] * _d_gtransfxs_d_temp[_qp][i + _group * _num_groups] * _phi[_j][_qp] * computeConcentration((*_group_fluxes[i]), _qp);
    }
  }

  return jac;
}
