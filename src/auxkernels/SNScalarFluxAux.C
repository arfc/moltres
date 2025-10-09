#include "SNScalarFluxAux.h"
#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNScalarFluxAux);

InputParameters
SNScalarFluxAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredCoupledVar("psi", "SN angular flux array variable");
  return params;
}

SNScalarFluxAux::SNScalarFluxAux(const InputParameters & parameters)
  : AuxKernel(parameters),
  _psi(coupledArrayValue("psi"))
{
  // Level-symmetric quadrature weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(getParam<unsigned int>("N"));
  _weights = ords_weights.col(3);
}

Real
SNScalarFluxAux::computeValue()
{
  return _weights.transpose() * _psi[_qp];
}
