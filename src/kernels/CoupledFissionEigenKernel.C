#include "CoupledFissionEigenKernel.h"

template<>
InputParameters validParams<CoupledFissionEigenKernel>()
{
  InputParameters params = validParams<EigenKernel>();
  params.addRequiredParam<int>("group_number", "The current energy group");
  params.addRequiredParam<int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("temperature", "The temperature used to interpolate material properties");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. These MUST be listed by decreasing energy/increasing group number.");
  return params;
}

CoupledFissionEigenKernel::CoupledFissionEigenKernel(const InputParameters & parameters) :
    EigenKernel(parameters),
    _nsf(getMaterialProperty<std::vector<Real> >("nsf")),
    _d_nsf_d_temp(getMaterialProperty<std::vector<Real> >("d_nsf_d_temp")),
    _chi(getMaterialProperty<std::vector<Real> >("chi")),
    _d_chi_d_temp(getMaterialProperty<std::vector<Real> >("d_chi_d_temp")),
    _group(getParam<int>("group_number") - 1),
    _num_groups(getParam<int>("num_groups")),
    _temp_id(coupled("temperature"))
{
  int n = coupledComponents("coupled_vars");
  if (!(n == _num_groups))
  {
    std::cerr << "The number of coupled variables doesn't match the number of groups." << std::endl;
    std::abort();
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
CoupledFissionEigenKernel::computeQpResidual()
{
  Real r = 0;
  for (int i = 0; i < _num_groups; ++i)
  {
    r += -_test[_i][_qp] * _chi[_qp][_group] * _nsf[_qp][i] * (*_group_fluxes[i])[_qp];
  }

  return r;
}

Real
CoupledFissionEigenKernel::computeQpJacobian()
{
  Real jac = 0;
  for (int i = 0; i < _num_groups; ++i)
  {
    if (i == _group)
    {
      jac += -_test[_i][_qp] * _chi[_qp][_group] * _nsf[_qp][i] * _phi[_j][_qp];
      break;
    }
  }

  return jac;
}

Real
CoupledFissionEigenKernel::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i])
    {
      jac += -_test[_i][_qp] * _chi[_qp][_group] * _nsf[_qp][i] * _phi[_j][_qp];
      break;
    }
  }

  if (jvar == _temp_id)
    for (int i = 0; i < _num_groups; ++i)
      jac += -_test[_i][_qp] * (*_group_fluxes[i])[_qp] * (_d_chi_d_temp[_qp][_group] * _phi[_j][_qp] * _nsf[_qp][i] + _chi[_qp][_group] * _d_nsf_d_temp[_qp][i] * _phi[_j][_qp]);

  return jac;
}
