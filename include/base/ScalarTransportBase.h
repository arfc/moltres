#ifndef SCALARTRANSPORTBASE_H
#define SCALARTRANSPORTBASE_H

#include "MooseObject.h"
#include "InputParameters.h"

template<typename T>
class ScalarTransportBase : public T
{
public:
  ScalarTransportBase(const InputParameters & parameters);

  Real computeConcentration();
  RealVectorValue computeConcentrationGradient();
  Real computeConcentrationDerivative();
  RealVectorValue computeConcentrationGradientDerivative();
  Real computeConcentrationDot();
  Real computeConcentrationDotDerivative();
  static InputParameters validParams();

private:
  const bool _use_exp_form;
};

template<typename T>
ScalarTransportBase<T>::ScalarTransportBase(const InputParameters & parameters) :
    T(parameters),
    _use_exp_form(MooseObject::getParam<bool>("use_exp_form"))
{
}

template<typename T>
InputParameters
ScalarTransportBase<T>::validParams()
{
  InputParameters params = emptyInputParameters();
  params.addParam<bool>("use_exp_form", true, "Whether concentrations should be in an expotential/logarithmic format.");
  return params;
}

template<typename T>
Real
ScalarTransportBase<T>::computeConcentration()
{
  if (_use_exp_form)
    return std::exp(this->_u[this->_qp]);
  else
    return this->_u[this->_qp];
}

template<typename T>
RealVectorValue
ScalarTransportBase<T>::computeConcentrationGradient()
{
  if (_use_exp_form)
    return std::exp(this->_u[this->_qp]) * this->_grad_u[this->_qp];
  else
    return this->_grad_u[this->_qp];
}

template<typename T>
Real
ScalarTransportBase<T>::computeConcentrationDerivative()
{
  if (_use_exp_form)
    return std::exp(this->_u[this->_qp]) * this->_phi[this->_j][this->_qp];
  else
    return this->_phi[this->_j][this->_qp];
}

template<typename T>
RealVectorValue
ScalarTransportBase<T>::computeConcentrationGradientDerivative()
{
  if (_use_exp_form)
    return std::exp(this->_u[this->_qp]) * this->_grad_phi[this->_j][this->_qp] +
      std::exp(this->_u[this->_qp]) * this->_phi[this->_j][this->_qp] * this->_grad_u[this->_qp];

  else
    return this->_grad_phi[this->_j][this->_qp];
}

template<typename T>
Real
ScalarTransportBase<T>::computeConcentrationDot()
{
  if (_use_exp_form)
    return std::exp(this->_u[this->_qp]) * this->_u_dot[this->_qp];
  else
    return this->_u_dot[this->_qp];
}

template<typename T>
Real
ScalarTransportBase<T>::computeConcentrationDotDerivative()
{
  if (_use_exp_form)
    return std::exp(this->_u[this->_qp]) * this->_phi[this->_j][this->_qp] * this->_u_dot[this->_qp] +
      std::exp(this->_u[this->_qp]) * this->_du_dot_du[this->_qp] * this->_phi[this->_j][this->_qp];
  else
    return this->_du_dot_du[this->_qp] * this->_phi[this->_j][this->_qp];
}

#endif //SCALARTRANSPORTBASE_H
