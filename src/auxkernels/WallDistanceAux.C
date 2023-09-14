#include "WallDistanceAux.h"

registerMooseObject("MoltresApp", WallDistanceAux);

InputParameters
WallDistanceAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Computes the minimum wall distance of each node from wall "
                             "boundaries");
  params.addRequiredParam<std::vector<BoundaryName>>(
    "walls",
    "Boundaries that correspond to solid walls");
  return params;
}

WallDistanceAux::WallDistanceAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _wall_boundary_names(getParam<std::vector<BoundaryName>>("walls"))
{
  const MeshBase & mesh = _subproblem.mesh().getMesh();
  if (!mesh.is_replicated())
    mooseError("WallDistanceAux only supports replicated meshes");
  if (!isNodal())
    mooseError("WallDistanceAux only works on nodal wall distance variable fields "
               "(e.g. LAGRANGE).");
}

Real
WallDistanceAux::computeValue()
{
  // Get the ids of the wall boundaries
  std::vector<BoundaryID> vec_ids = _mesh.getBoundaryIDs(_wall_boundary_names, true);

  // Loop over all boundary nodes
  Real min_dist = 1e9;
  for (const auto & bnode : *_mesh.getBoundaryNodeRange())
  {
    BoundaryID bnd_id = bnode->_bnd_id;
    // Loop over all boundary ids
    for (BoundaryID bid : vec_ids)
    {
      // Check if boundary node falls on wall boundary
      if (bid == bnd_id)
      {
        // Find distance to closest wall boundary node
        Node * node = bnode->_node;
        const auto dist = (*node - *_current_node).norm();
        min_dist = std::min(min_dist, dist);
      }
    }
  }
  return min_dist;
}
