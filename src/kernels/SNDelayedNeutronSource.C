#include "SNDelayedNeutronSource.h"
#include "Executioner.h"

registerMooseObject("MoltresApp", SNDelayedNeutronSource);

InputParameters
SNDelayedNeutronSource::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredCoupledVar("delayed_neutron_source",
                               "Delayed neutron source term variable name");
  return params;
}

SNDelayedNeutronSource::SNDelayedNeutronSource(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _decay_constant(getMaterialProperty<std::vector<Real>>("decay_constant")),
    _chi_d(getMaterialProperty<std::vector<Real>>("chi_d")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _delayed_source(coupledValue("delayed_neutron_source"))
{
  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNDelayedNeutronSource::computeQpResidual(RealEigenVector & residual)
{
  if (_chi_d[_qp][_group] == 0.0)
    return;

  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);

  Real rhs = 0.125 * _chi_d[_qp][_group] * _delayed_source[_qp];

  residual = -_weights.cwiseProduct(lhs) * rhs;
}
