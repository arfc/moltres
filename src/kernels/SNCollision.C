#include "SNCollision.h"

registerMooseObject("MoltresApp", SNCollision);

InputParameters
SNCollision::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}

SNCollision::SNCollision(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _sigma_t(getMaterialProperty<std::vector<Real>>("sigma_t")),
    _d_sigma_t_d_temp(getMaterialProperty<std::vector<Real>>("d_sigma_t_d_temp")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _temp_id(coupled("temperature"))
{
}

void
SNCollision::computeQpResidual(RealEigenVector & residual)
{
  residual = _test[_i][_qp] * _sigma_t[_qp][_group] * _u[_qp];
}

RealEigenVector
SNCollision::computeQpJacobian()
{
  return RealEigenVector::Constant(
      _var.count(), _test[_i][_qp] * _sigma_t[_qp][_group] * _phi[_j][_qp]);
}

RealEigenMatrix
SNCollision::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  if (jvar.number() == _temp_id)
    return (_test[_i][_qp] * _d_sigma_t_d_temp[_qp][_group] *
            _phi[_j][_qp] * _u[_qp]).asDiagonal();
  else
    return ArrayKernel::computeQpOffDiagJacobian(jvar);
}
