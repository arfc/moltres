#include "SATurbulentViscosity.h"

#include "metaphysicl/raw_type.h"

registerMooseObject("MoltresApp", SATurbulentViscosity);

InputParameters
SATurbulentViscosity::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("Adds all terms in the turbulent viscosity equation from the "
                             "Spalart-Allmaras model");
  params.addRequiredCoupledVar("velocity", "The velocity vector variable");
  params.addParam<MaterialPropertyName>("mu_name", "mu",
                                        "The name of the dynamic viscosity material property");
  params.addParam<MaterialPropertyName>("rho_name", "rho",
                                        "The name of the density material property");
  params.addRequiredCoupledVar("wall_distance_var", "Wall distance aux variable name");
  return params;
}

SATurbulentViscosity::SATurbulentViscosity(const InputParameters & parameters)
  : ADKernel(parameters),
    _velocity(adCoupledVectorValue("velocity")),
    _grad_velocity(adCoupledVectorGradient("velocity")),
    _mu(getADMaterialProperty<Real>("mu_name")),
    _rho(getADMaterialProperty<Real>("rho_name")),
    _tau_visc(getADMaterialProperty<Real>("tau_viscosity")),
    _wall_dist(coupledValue("wall_distance_var")),
    _sigma(2./3.),
    _cb1(0.1355),
    _cb2(0.622),
    _kappa(0.41),
    _cw1(_cb1 / _kappa / _kappa + (1 + _cb2) / _sigma),
    _cw2(0.3),
    _cw3(2.),
    _cv1(7.1),
    _ct1(1.),
    _ct2(2.),
    _ct3(1.2),
    _ct4(0.5)
{
}

ADReal
SATurbulentViscosity::computeQpResidual()
{
  ADRealTensorValue vorticity = .5 * (_grad_velocity[_qp] - _grad_velocity[_qp].transpose());
  ADReal vorticity_mag = 0.;
  for (unsigned int i = 0; i < 3; i++)
    for (unsigned int j = 0; j < 3; j++)
      vorticity_mag += 2 * vorticity(i, j) * vorticity(i, j);
  vorticity_mag += 1e-16;
  
  ADRealTensorValue strain = .5 * (_grad_velocity[_qp] + _grad_velocity[_qp].transpose());
  ADReal strain_mag = 0.;
  for (unsigned int i = 0; i < 3; i++)
    for (unsigned int j = 0; j < 3; j++)
      strain_mag += 2 * strain(i, j) * strain(i, j);
  strain_mag += 1e-16;

  Real d = std::max(_wall_dist[_qp], 1e-16);
  ADReal chi = _u[_qp] / _mu[_qp];
  ADReal fv1 = std::pow(chi, 3) / (std::pow(chi, 3) + std::pow(_cv1, 3));
  ADReal fv2 = 1. - chi / (1. + chi * fv1);
  ADReal Omega = std::sqrt(vorticity_mag);
  ADReal S_ij = std::sqrt(strain_mag);
//  ADReal Sbar = _u[_qp] * fv2 / (_kappa * _kappa * d * d * _rho[_qp]);
//  ADReal S;
//  if (Sbar >= -.7 * Omega)
//    S = Omega + Sbar;
//  else
//    S = Omega + (Omega * (.7 * .7 * Omega + .9 * Sbar)) / ((.9 - 2 * .7) * Omega - Sbar);
//  ADReal S = Omega + _u[_qp] * fv2 / (_kappa * _kappa * d * d * _rho[_qp]);
  ADReal S = Omega + 2 * std::min(0., S_ij - Omega) + _u[_qp] * fv2 / \
             (_kappa * _kappa * d * d * _rho[_qp]);
  ADReal r;
  if (S <= 0.)
    r = 10.;
  else
    r = std::min(_u[_qp] / (S * _kappa * d * d * _rho[_qp]), 10.);
  ADReal g = r + _cw2 * (std::pow(r, 6) - r);
  ADReal fw = g * (1. + std::pow(_cw3, 6)) / (std::pow(g, 6) + std::pow(_cw3, 6));
//  ADReal ft2 = _ct3 * std::exp(-_ct4 * chi * chi);

  ADReal convection_part = _rho[_qp] * _velocity[_qp] * _grad_u[_qp] * _test[_i][_qp];
  ADReal source_part = -(1) * _rho[_qp] * _cb1 * S * _u[_qp] * _test[_i][_qp];
  ADReal destruction_part = (_cw1 * fw) * std::pow(_u[_qp] / d, 2) * _test[_i][_qp];
//  ADReal destruction_part = (_cw1 * fw - _cb1 * ft2 / _kappa / _kappa) * std::pow(_u[_qp] / d, 2) \
                            * _test[_i][_qp];
  ADReal diffusion_part = 1. / _sigma * ((_mu[_qp] + _u[_qp]) * \
                        _grad_u[_qp] * _grad_test[_i][_qp] - _cb2 * _grad_u[_qp] * _grad_u[_qp] * \
                        _test[_i][_qp]);
  ADReal normal_part = convection_part + source_part + destruction_part + diffusion_part;

  ADReal psi_supg = _tau_visc[_qp] * _vel_elem * _grad_test[_i][_qp];
  ADReal transient_supg = _bTransient ? _rho[_qp] * _var.uDot()[_qp] : 0;
  ADReal convection_supg = _rho[_qp] * _velocity[_qp] * _grad_u[_qp];
  ADReal source_supg = _rho[_qp] * _cb1 * (Omega + _u[_qp] * fv2 / _kappa / _kappa / d / d) * \
                     _u[_qp];
  ADReal destruction_supg = -_cw1 * fw * std::pow(_u[_qp] / d, 2);
  ADReal diffusion_supg = 1. / _sigma;

  ADReal res = normal_part;
  return res;
}
