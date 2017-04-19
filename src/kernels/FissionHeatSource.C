#include "FissionHeatSource.h"

template <>
InputParameters
validParams<FissionHeatSource>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<unsigned int>("num_groups", "The total numer of energy groups");
  params.addRequiredCoupledVar("group_fluxes", "All the variables that hold the group fluxes. "
                                               "These MUST be listed by decreasing "
                                               "energy/increasing group number.");
  params.addRequiredParam<PostprocessorName>(
      "tot_fissions", "The total fission postprocessor that's used to normalize the heat source.");
  params.addRequiredParam<Real>("power", "The reactor power.");
  return params;
}

FissionHeatSource::FissionHeatSource(const InputParameters & parameters)
  : Kernel(parameters),
    _fissxs(getMaterialProperty<std::vector<Real>>("fissxs")),
    _d_fissxs_d_temp(getMaterialProperty<std::vector<Real>>("d_fissxs_d_temp")),
    _num_groups(getParam<unsigned int>("num_groups")),
    _tot_fissions(getPostprocessorValue("tot_fissions")),
    _power(getParam<Real>("power"))
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
FissionHeatSource::computeQpResidual()
{
  Real r = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    r += -_test[_i][_qp] * _fissxs[_qp][i] * (*_group_fluxes[i])[_qp] * _power / _tot_fissions;
  }

  return r;
}

Real
FissionHeatSource::computeQpJacobian()
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    jac += -_test[_i][_qp] * _d_fissxs_d_temp[_qp][i] * (*_group_fluxes[i])[_qp] * _power /
           _tot_fissions;
  }

  return jac;
}

Real
FissionHeatSource::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0;
  for (unsigned int i = 0; i < _num_groups; ++i)
  {
    if (jvar == _flux_ids[i])
    {
      jac += -_test[_i][_qp] * _fissxs[_qp][i] * _phi[_j][_qp] * _power / _tot_fissions;
      break;
    }
  }

  return jac;
}
