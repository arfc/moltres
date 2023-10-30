#include "DGTurbulentDiffusion.h"
#include "metaphysicl/raw_type.h"

using MetaPhysicL::raw_value;

registerMooseObject("MoltresApp", DGTurbulentDiffusion);

InputParameters
DGTurbulentDiffusion::validParams()
{
  InputParameters params = DGKernel::validParams();
  params.addClassDescription(
      "Adds the turbulent diffusion term for delayed "
      "neutron precursors using the discontinuous Galerkin method.");
  params.addParam<Real>("sigma", 6, "sigma");
  params.addParam<Real>("epsilon", -1, "epsilon");
  params.addParam<MaterialPropertyName>(
      "mu_name", "mu", "The name of the dynamic viscosity material proerty");
  params.addParam<MaterialPropertyName>(
      "rho_name", "rho", "The name of the density material property");
  params.addRequiredCoupledVar("mu_tilde", "Spalart-Allmaras turbulence viscosity variable");
  params.addParam<Real>("schmidt_number", 0.85, "Turbulent Schmidt number");
  return params;
}

DGTurbulentDiffusion::DGTurbulentDiffusion(const InputParameters & parameters)
  : DGKernel(parameters),
    _epsilon(getParam<Real>("epsilon")),
    _sigma(getParam<Real>("sigma")),
    _mu(getADMaterialProperty<Real>("mu_name")),
    _mu_nb(getNeighborADMaterialProperty<Real>("mu_name")),
    _rho(getADMaterialProperty<Real>("rho_name")),
    _rho_nb(getNeighborADMaterialProperty<Real>("rho_name")),
    _mu_tilde(adCoupledValue("mu_tilde")),
    _mu_tilde_nb(adCoupledNeighborValue("mu_tilde")),
    _sc(getParam<Real>("schmidt_number")),
    _mu_tilde_var_number(coupled("mu_tilde"))
{
}

Real
DGTurbulentDiffusion::computeQpResidual(Moose::DGResidualType type)
{
  Real chi = raw_value(_mu_tilde[_qp] / _mu[_qp]);
  Real cv1 = 7.1;
  Real fv1 = Utility::pow<3>(chi) / (Utility::pow<3>(chi) + Utility::pow<3>(cv1));
  // Turbulent diffusion coefficient
  Real D = raw_value(_mu_tilde[_qp] / _rho[_qp]) * fv1 / _sc;

  Real chi_nb = raw_value(_mu_tilde_nb[_qp]) / raw_value(_mu_nb[_qp]);
  Real cv1_nb = 7.1;
  Real fv1_nb = Utility::pow<3>(chi_nb) / (Utility::pow<3>(chi_nb) + Utility::pow<3>(cv1_nb));
  // Neighbor turbulent diffusion coefficient
  Real D_nb = raw_value(_mu_tilde_nb[_qp] / _rho_nb[_qp]) * fv1_nb / _sc;

  Real r = 0.;

  const int elem_b_order = std::max(libMesh::Order(1), _var.order());
  const Real h_elem =
    _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);

  switch (type)
  {
    case Moose::Element:
      r -= 0.5 *
           (D * _grad_u[_qp] * _normals[_qp] +
            D_nb * _grad_u_neighbor[_qp] * _normals[_qp]) *
           _test[_i][_qp];
      r += _epsilon * 0.5 * (_u[_qp] - _u_neighbor[_qp]) * D * _grad_test[_i][_qp] *
           _normals[_qp];
      r += _sigma / h_elem * (_u[_qp] - _u_neighbor[_qp]) * _test[_i][_qp];
      break;

    case Moose::Neighbor:
      r += 0.5 *
           (D * _grad_u[_qp] * _normals[_qp] +
            D_nb * _grad_u_neighbor[_qp] * _normals[_qp]) *
           _test_neighbor[_i][_qp];
      r += _epsilon * 0.5 * (_u[_qp] - _u_neighbor[_qp]) * D_nb *
           _grad_test_neighbor[_i][_qp] * _normals[_qp];
      r -= _sigma / h_elem * (_u[_qp] - _u_neighbor[_qp]) * _test_neighbor[_i][_qp];
      break;
  }

  return r;
}

Real
DGTurbulentDiffusion::computeQpJacobian(Moose::DGJacobianType type)
{
  Real chi = raw_value(_mu_tilde[_qp] / _mu[_qp]);
  Real cv1 = 7.1;
  Real fv1 = Utility::pow<3>(chi) / (Utility::pow<3>(chi) + Utility::pow<3>(cv1));
  // Turbulent diffusion coefficient
  Real D = raw_value(_mu_tilde[_qp] / _rho[_qp]) * fv1 / _sc;

  Real chi_nb = raw_value(_mu_tilde_nb[_qp]) / raw_value(_mu_nb[_qp]);
  Real cv1_nb = 7.1;
  Real fv1_nb = Utility::pow<3>(chi_nb) / (Utility::pow<3>(chi_nb) + Utility::pow<3>(cv1_nb));
  // Neighbor turbulent diffusion coefficient
  Real D_nb = raw_value(_mu_tilde_nb[_qp] / _rho_nb[_qp]) * fv1_nb / _sc;

  Real r = 0.0;

  const int elem_b_order = std::max(libMesh::Order(1), _var.order());
  const Real h_elem =
      _current_elem_volume / _current_side_volume * 1.0 / Utility::pow<2>(elem_b_order);

  switch (type)
  {
    case Moose::ElementElement:
      r -= 0.5 * D * _grad_phi[_j][_qp] * _normals[_qp] * _test[_i][_qp];
      r += _epsilon * 0.5 * _phi[_j][_qp] * D * _grad_test[_i][_qp] * _normals[_qp];
      r += _sigma / h_elem * _phi[_j][_qp] * _test[_i][_qp];
      break;

    case Moose::ElementNeighbor:
      r -= 0.5 * D_nb * _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test[_i][_qp];
      r += _epsilon * 0.5 * -_phi_neighbor[_j][_qp] * D * _grad_test[_i][_qp] *
           _normals[_qp];
      r += _sigma / h_elem * -_phi_neighbor[_j][_qp] * _test[_i][_qp];
      break;

    case Moose::NeighborElement:
      r += 0.5 * D * _grad_phi[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp];
      r += _epsilon * 0.5 * _phi[_j][_qp] * D_nb * _grad_test_neighbor[_i][_qp] *
           _normals[_qp];
      r -= _sigma / h_elem * _phi[_j][_qp] * _test_neighbor[_i][_qp];
      break;

    case Moose::NeighborNeighbor:
      r += 0.5 * D_nb * _grad_phi_neighbor[_j][_qp] * _normals[_qp] *
           _test_neighbor[_i][_qp];
      r += _epsilon * 0.5 * -_phi_neighbor[_j][_qp] * D_nb *
           _grad_test_neighbor[_i][_qp] * _normals[_qp];
      r -= _sigma / h_elem * -_phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp];
      break;
  }

  return r;
}

Real
DGTurbulentDiffusion::computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned jvar)
{
  Real r = 0.0;

  if (jvar == _mu_tilde_var_number)
  {
    Real chi = raw_value(_mu_tilde[_qp] / _mu[_qp]);
    Real cv1 = 7.1;
    Real fv1 = Utility::pow<3>(chi) / (Utility::pow<3>(chi) + Utility::pow<3>(cv1));
    // Turbulent diffusion coefficient
    Real D = _phi[_j][_qp] * fv1 / raw_value(_rho[_qp]) / _sc *
      (1 + 3 * Utility::pow<3>(cv1) / (Utility::pow<3>(chi) + Utility::pow<3>(cv1)));

    Real chi_nb = raw_value(_mu_tilde_nb[_qp] / _mu_nb[_qp]);
    Real fv1_nb = Utility::pow<3>(chi_nb) / (Utility::pow<3>(chi_nb) + Utility::pow<3>(cv1));
    // Neighbor turbulent diffusion coefficient
    Real D_nb = _phi_neighbor[_j][_qp] * fv1_nb / raw_value(_rho_nb[_qp]) / _sc *
      (1 + 3 * Utility::pow<3>(cv1) / (Utility::pow<3>(chi_nb) + Utility::pow<3>(cv1)));

    const int elem_b_order = std::max(libMesh::Order(1), _var.order());
    const Real h_elem =
        _current_elem_volume / _current_side_volume * 1.0 / Utility::pow<2>(elem_b_order);

    switch (type)
    {
      case Moose::ElementElement:
        r -= 0.5 * D * _grad_u[_qp] * _normals[_qp] * _test[_i][_qp];
        r += _epsilon * 0.5 * _u[_qp] * D * _grad_test[_i][_qp] * _normals[_qp];
        break;

      case Moose::ElementNeighbor:
        r -= 0.5 * D_nb * _grad_u_neighbor[_qp] * _normals[_qp] * _test[_i][_qp];
        r += _epsilon * 0.5 * -_u_neighbor[_qp] * D * _grad_test[_i][_qp] *
             _normals[_qp];
        break;

      case Moose::NeighborElement:
        r += 0.5 * D * _grad_u[_qp] * _normals[_qp] * _test_neighbor[_i][_qp];
        r += _epsilon * 0.5 * _u[_qp] * D_nb * _grad_test_neighbor[_i][_qp] *
             _normals[_qp];
        break;

      case Moose::NeighborNeighbor:
        r += 0.5 * D_nb * _grad_u_neighbor[_qp] * _normals[_qp] *
             _test_neighbor[_i][_qp];
        r += _epsilon * 0.5 * -_u_neighbor[_qp] * D_nb *
             _grad_test_neighbor[_i][_qp] * _normals[_qp];
        break;
    }
  }

  return r;
}
