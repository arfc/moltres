#include "WallShearStressAux.h"
#include "Assembly.h"

using MetaPhysicL::raw_value;

registerMooseObject("MoltresApp", WallShearStressAux);

InputParameters
WallShearStressAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Computes the wall shear stress along the given boundary.");
  params.addRequiredCoupledVar("velocity", "The velocity");
  params.addParam<MaterialPropertyName>("mu_name", "mu", "The name of the dynamic viscosity");
  return params;
}

WallShearStressAux::WallShearStressAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _grad_velocity(adCoupledVectorGradient("velocity")),
    _mu(getADMaterialProperty<Real>("mu_name")),
    _normals(_assembly.normals())
{
  if (!isParamValid("boundary"))
    paramError("boundary",
               "The wall boundary must be provided to calculate the wall shear stress.");
  if (_var.feType().family != MONOMIAL || _var.feType().order != CONSTANT)
    mooseError("WallShearStressAux is compatible with CONSTANT MONOMIAL "
               "wall shear stress variable only.");
}

Real
WallShearStressAux::computeValue()
{
  RealVectorValue grad_vel_wall(0., 0., 0.);
  // Velocity gradient along normal to wall (pointing in)
  for (unsigned int i = 0; i < 3; i++)
    for (unsigned int j = 0; j < 3; j++)
      grad_vel_wall(i) -= raw_value(_grad_velocity[_qp](i, j)) * _normals[_qp](j);
  // Parallel component of prior velocity gradient
  grad_vel_wall -= (_normals[_qp] * grad_vel_wall) * _normals[_qp];

  return raw_value(_mu[_qp]) * grad_vel_wall.norm();
}
