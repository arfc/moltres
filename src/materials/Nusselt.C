#include "Nusselt.h"

registerMooseObject("MoltresApp", Nusselt);

InputParameters
Nusselt::validParams()
{
  InputParameters params = Material::validParams();
  params.addParam<MaterialPropertyName>("k_name", "k", "The name of the thermal conductivity");
  params.addRequiredParam<Real>("L", "The channel radius");
  return params;
}

Nusselt::Nusselt(const InputParameters & parameters)
  : Material(parameters),
    _k(getMaterialProperty<Real>("k_name")),
    _l_value(getParam<Real>("L")),
    _nu(declareProperty<Real>("nu")),
    _h(declareProperty<Real>("h"))
{
}

void
Nusselt::computeQpProperties()
{
  // Forced laminar flow; J. Křepel et al. / Annals of Nuclear Energy 34 (2007) 449–462
  _nu[_qp] = 4.;
  _h[_qp] = _nu[_qp] * _k[_qp] / _l_value;
}
