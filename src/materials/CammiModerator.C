#include "CammiModerator.h"
#include "MooseUtils.h"
// #define PRINT(var) #var

registerMooseObject("MoltresApp", CammiModerator);

template <>
InputParameters
validParams<CammiModerator>()
{
  InputParameters params = validParams<GenericMoltresMaterial>();
  return params;
}

CammiModerator::CammiModerator(const InputParameters & parameters)
  : GenericMoltresMaterial(parameters), _k(declareProperty<Real>("k"))
{
}

void
CammiModerator::computeQpProperties()
{
  GenericMoltresMaterial::computeQpProperties();

  _k[_qp] = 3763. / 1e2 * std::pow(_temperature[_qp], -0.7); // units of W/(cm*K); temperature in K
}
