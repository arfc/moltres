#include "MatINSTemperatureTimeDerivative.h"

template<>
InputParameters validParams<MatINSTemperatureTimeDerivative>()
{
  InputParameters params = validParams<TimeDerivative>();
  // params.addRequiredParam<MaterialPropertyName>("rho", "The material property name for the density");
  // params.addRequiredParam<MaterialPropertyName>("cp", "The material property name for the specific heat");
  return params;
}


MatINSTemperatureTimeDerivative::MatINSTemperatureTimeDerivative(const InputParameters & parameters) :
  TimeDerivative(parameters),
    _rho(getMaterialProperty<Real>("rho")),
    _cp(getMaterialProperty<Real>("cp"))
{}



Real
MatINSTemperatureTimeDerivative::computeQpResidual()
{
  return _rho[_qp] * _cp[_qp] * TimeDerivative::computeQpResidual();
}



Real
MatINSTemperatureTimeDerivative::computeQpJacobian()
{
  return _rho[_qp] * _cp[_qp] * TimeDerivative::computeQpJacobian();
}



Real MatINSTemperatureTimeDerivative::computeQpOffDiagJacobian(unsigned)
{
  return 0.;
}
