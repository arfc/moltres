#include "MatDiffusionAux.h"

template<>
InputParameters validParams<MatDiffusionAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<MaterialPropertyName>("prop_name", "the name of the material property we are going to use");
  params.addRequiredCoupledVar("diffuse_var", "The variable that's diffusing.");
  return params;
}


MatDiffusionAux::MatDiffusionAux(const InputParameters & parameters) :
    AuxKernel(parameters),
    _grad_diffuse_var(coupledGradient("diffuse_var"))
{
    _diff = &getMaterialProperty<Real>("prop_name");
}

Real
MatDiffusionAux::computeValue()
{
  return (*_diff)[_qp] * _grad_diffuse_var[_qp].norm();
}
