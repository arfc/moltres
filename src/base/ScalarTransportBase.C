#include "ScalarTransportBase.h"

template <>
InputParameters
validParams<ScalarTransportBase>()
{
  InputParameters params = emptyInputParameters();
  params.addParam<bool>("use_exp_form",
                        true,
                        "Whether concentrations should be in an expotential/logarithmic format.");
  return params;
}

ScalarTransportBase::ScalarTransportBase(const InputParameters & parameters)
  : _use_exp_form(parameters.get<bool>("use_exp_form"))
{
}

Real
ScalarTransportBase::computeConcentration(const VariableValue & u, unsigned int qp)
{
  if (_use_exp_form)
    return std::exp(u[qp]);
  else
    return u[qp];
}

RealVectorValue
ScalarTransportBase::computeConcentrationGradient(const VariableValue & u,
                                                  const VariableGradient & grad_u,
                                                  unsigned int qp)
{
  if (_use_exp_form)
    return std::exp(u[qp]) * grad_u[qp];
  else
    return grad_u[qp];
}

Real
ScalarTransportBase::computeConcentrationDerivative(const VariableValue & u,
                                                    const VariablePhiValue & phi,
                                                    unsigned int j,
                                                    unsigned int qp)
{
  if (_use_exp_form)
    return std::exp(u[qp]) * phi[j][qp];
  else
    return phi[j][qp];
}

RealVectorValue
ScalarTransportBase::computeConcentrationGradientDerivative(const VariableValue & u,
                                                            const VariableGradient & grad_u,
                                                            const VariablePhiValue & phi,
                                                            const VariablePhiGradient & grad_phi,
                                                            unsigned int j,
                                                            unsigned int qp)
{
  if (_use_exp_form)
    return std::exp(u[qp]) * grad_phi[j][qp] + std::exp(u[qp]) * phi[j][qp] * grad_u[qp];

  else
    return grad_phi[j][qp];
}

Real
ScalarTransportBase::computeConcentrationDot(const VariableValue & u,
                                             const VariableValue u_dot,
                                             unsigned int qp)
{
  if (_use_exp_form)
    return std::exp(u[qp]) * u_dot[qp];

  else
    return u_dot[qp];
}

Real
ScalarTransportBase::computeConcentrationDotDerivative(const VariableValue & u,
                                                       const VariableValue & u_dot,
                                                       const VariableValue & du_dot_du,
                                                       const VariablePhiValue & phi,
                                                       unsigned int j,
                                                       unsigned int qp)
{
  if (_use_exp_form)
    return std::exp(u[qp]) * phi[j][qp] * u_dot[qp] + std::exp(u[qp]) * du_dot_du[qp] * phi[j][qp];

  else
    return du_dot_du[qp] * phi[j][qp];
}
