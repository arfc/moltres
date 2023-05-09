#include "SATurbulentViscosity.h"

registerMooseObject("MoltresApp", SATurbulentViscosity);

InputParameters
SATurbulentViscosity::validParams()
{
  InputParameters params = INSFEFluidKernelBase::validParams();
  params.addClassDescription("Adds all terms in the turbulent viscosity equation from the "
                             "Spalart-Allmaras model");
  params.addParam<std::vector<BoundaryName>>("walls", "Boundaries that correspond to solid walls");
  return params;
}

SATurbulentViscosity::SATurbulentViscosity(const InputParameters & parameters)
  : INSFEFluidKernelBase(parameters),
    _wall_boundary_names(getParam<std::vector<BoundaryName>>("walls")),
    _sigma(2/3),
    _cb1(0.1355),
    _cb2(0.622),
    _kappa(0.41),
    _cw1(_cb1 / _kappa / _kappa + (1 + _cb2) / _sigma),
    _cw2(0.3),
    _cw3(2),
    _cv1(7.1),
    _ct1(1),
    _ct2(2),
    _ct3(1.2),
    _ct4(0.5)
{
}

Real
SATurbulentViscosity::computeQpResidual()
{
  // Get reference to the libMesh mesh object
  const MeshBase & l_mesh = _mesh.getMesh();

  // Get the ids of the wall boundaries
  std::vector<BoundaryID> vec_ids = _mesh.getBoundaryIDs(_wall_boundary_names, true);

  // Loop over all boundaries
  Real min_dist2 = 1e9;
  const auto & bnd_to_elem_map = _mesh.getBoundariesToActiveSemiLocalElemIds();
  for (BoundaryID bid : vec_ids)
  {
    // Get the set of elements on this boundary
    auto search = bnd_to_elem_map.find(bid);
    if (search == bnd_to_elem_map.end())
      mooseError("Error computing wall distance; the boundary id ", bid, " is invalid");
    const auto & bnd_elems = search->second;

    // Loop over all boundary elements and find the distance to the closest one
    for (dof_id_type elem_id : bnd_elems)
    {
      const Elem & elem{l_mesh.elem_ref(elem_id)};
      const auto side = _mesh.sideWithBoundaryID(&elem, bid);
      const FaceInfo * fi = _mesh.faceInfo(&elem, side);
      // It's possible that we are on an internal boundary
      if (!fi)
      {
        const Elem * const neigh = elem.neighbor_ptr(side);
        mooseAssert(
            neigh,
            "In WallDistanceMixingLengthAux, we could not find a face information object with elem "
            "and side, and we are on an external boundary. This shouldn't happen.");
        const auto neigh_side = neigh->which_neighbor_am_i(&elem);
        fi = _mesh.faceInfo(neigh, neigh_side);
        mooseAssert(fi, "We should have a face info for either the elem or neigh side");
      }
      Point bnd_pos = fi->faceCentroid();
      const auto distance = bnd_pos - _q_point[_qp];
      const auto dist2 = distance * distance;
      mooseAssert(dist2 != 0, "This distance should never be 0");
      min_dist2 = std::min(min_dist2, dist2);
    }
  }

  RealTensorValue vorticity(_grad_u_vel[_qp], _grad_v_vel[_qp], _grad_w_vel[_qp]);
  vorticity -= vorticity.transpose();
  Real vorticity_mag = 0.;
  for (unsigned int i = 0; i < 3; i++)
    for (unsigned int j = 0; j < 3; j++)
      vorticity_mag += 0.5 * vorticity(i, j) * vorticity(i, j);

  Real d = min_dist2;
  Real chi = _u[_qp] / _dynamic_viscosity[_qp] * _rho[_qp];
  Real fv1 = std::pow(chi, 3) / (std::pow(chi, 3) + std::pow(_cv1, 3));
  Real fv2 = 1 - chi / (1 + chi * fv1);
  Real Omega = std::sqrt(vorticity_mag);
  Real S = Omega + _u[_qp] * fv2 / (_kappa * _kappa * d * d);
  Real r = std::min(_u[_qp] / (S * _kappa * d * d), 10.);
  Real g = r + _cw2 * (std::pow(r, 6) - r);
  Real fw = g * (1 + std::pow(_cw3, 6)) / (std::pow(g, 6) + std::pow(_cw3, 6));

  RealVectorValue vec_vel(_u_vel[_qp], _v_vel[_qp], _w_vel[_qp]);

  Real convection_part = vec_vel * _grad_u[_qp] * _test[_i][_qp];
  Real source_part = _cb1 * (Omega + _u[_qp] * fv2 / _kappa / _kappa / d / d) * _u[_qp] * \
                     _test[_i][_qp];
  Real destruction_part = -_cw1 * fw * std::pow(_u[_qp] / d, 2) * _test[_i][_qp];
  Real diffusion_part = 1 / _sigma * ((_dynamic_viscosity[_qp] / _rho[_qp] + _u[_qp]) * \
                        _grad_u[_qp] * _grad_test[_i][_qp] + _cb2 * _grad_u[_qp] * _grad_u[_qp] * \
                        _test[_i][_qp]);
  return convection_part + source_part + destruction_part + diffusion_part;
}

Real
SATurbulentViscosity::computeQpJacobian()
{
  return 0.;
}

Real
SATurbulentViscosity::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.;
}
