#include "CammiFuel.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

template<>
InputParameters validParams<CammiFuel>()
{
  InputParameters params = validParams<GenericMoltresMaterial>();
  return params;
}


CammiFuel::CammiFuel(const InputParameters & parameters) :
    GenericMoltresMaterial(parameters),
    _rho(declareProperty<Real>("rho")),
    _k(declareProperty<Real>("k"))
{
}

void
CammiFuel::computeQpProperties()
{
  GenericMoltresMaterial::computeQpProperties();

  _rho[_qp] = 3374. / 1e6 * (1. - 1.9857e-4 * (_temperature[_qp] - 839)); // units of kg/cm^3; temperature in K
  _k[_qp] = (-3.70e-6 * std::pow(_temperature[_qp] - 273., 2) + 4.77e-3 * (_temperature[_qp] - 273.) - .309) / 1e2; // units of W/(cm*K); temperature in K
}
