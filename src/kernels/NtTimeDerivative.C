#include "NtTimeDerivative.h"
#include "Assembly.h"

// libmesh includes
#include "libmesh/quadrature.h"

template<>
InputParameters validParams<NtTimeDerivative>()
{
  InputParameters params = validParams<ScalarTransportTimeDerivative>();
  params.addRequiredParam<unsigned int>("group_number", "The group for which this kernel controls diffusion");
  params.addCoupledVar("temperature", "The temperature used to interpolate the diffusion coefficient");
  return params;
}

NtTimeDerivative::NtTimeDerivative(const InputParameters & parameters) :
    ScalarTransportTimeDerivative(parameters),
    _recipvel(getMaterialProperty<std::vector<Real> >("recipvel")),
    _d_recipvel_d_temp(getMaterialProperty<std::vector<Real> >("d_recipvel_d_temp")),
    _group(getParam<unsigned int>("group_number") - 1),
    _temp_id(coupled("temperature"))
{
}

Real
NtTimeDerivative::computeQpResidual()
{
  return ScalarTransportTimeDerivative::computeQpResidual() * _recipvel[_qp][_group];
}

Real
NtTimeDerivative::computeQpJacobian()
{
  return ScalarTransportTimeDerivative::computeQpJacobian() * _recipvel[_qp][_group];
}

Real
NtTimeDerivative::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _temp_id)
    return ScalarTransportTimeDerivative::computeQpResidual() * _d_recipvel_d_temp[_qp][_group] * _phi[_j][_qp];

  else
    return 0;
}
