#include "SNStreaming.h"

registerMooseObject("MoltresApp", SNStreaming);

InputParameters
SNStreaming::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}

SNStreaming::SNStreaming(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _tau_sn(getMaterialProperty<Real>("tau_sn")),
    _sigma_t(getMaterialProperty<std::vector<Real>>("sigma_t")),
    _d_sigma_t_d_temp(getMaterialProperty<std::vector<Real>>("d_sigma_t_d_temp")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _temp_id(coupled("temperature"))
{
  _ordinates = MoltresUtils::directions(_N);
}

void
SNStreaming::computeQpResidual(RealEigenVector & residual)
{
  RealEigenVector lhs = (_ordinates.cwiseProduct(_array_grad_test[_i][_qp])).rowwise().sum();
  RealEigenVector rhs = (_tau_sn[_qp] * (_ordinates.cwiseProduct(_grad_u[_qp])).rowwise().sum() -
                         ((1. - _tau_sn[_qp] * _sigma_t[_qp][_group]) * _u[_qp]));
  residual.noalias() = lhs.cwiseProduct(rhs);
}

RealEigenVector
SNStreaming::computeQpJacobian()
{
  RealEigenVector array_phi = RealEigenVector::Constant(_var.count(), _phi[_j][_qp]);
  RealEigenVector array_grad_phi;
  array_grad_phi << _grad_phi[_j][_qp](0),
                    _grad_phi[_j][_qp](1),
                    _grad_phi[_j][_qp](2);
  RealEigenVector lhs = (_ordinates.cwiseProduct(_array_grad_test[_i][_qp])).rowwise().sum();
  RealEigenVector rhs = (_tau_sn[_qp] * (_ordinates * array_grad_phi) -
                         ((1. - _tau_sn[_qp] * _sigma_t[_qp][_group]) * array_phi));
  return lhs.cwiseProduct(rhs);
}

RealEigenMatrix
SNStreaming::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  if (jvar.number() == _temp_id)
  {
    RealEigenVector lhs = (_ordinates.cwiseProduct(_array_grad_test[_i][_qp])).rowwise().sum();
    RealEigenVector rhs = (_tau_sn[_qp] * _d_sigma_t_d_temp[_qp][_group] * _phi[_j][_qp] * _u[_qp]);
    return (lhs.cwiseProduct(rhs)).asDiagonal();
  }
  else
    return ArrayKernel::computeQpOffDiagJacobian(jvar);
}
