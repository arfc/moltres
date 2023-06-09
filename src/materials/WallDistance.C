#include "WallDistance.h"

registerMooseObject("MoltresApp", WallDistance);

InputParameters
WallDistance::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Computes the minimum wall distance of each element from wall "
                             "boundaries as a material property");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "walls",
      "Boundaries that correspond to solid walls");
  return params;
}

WallDistance::WallDistance(const InputParameters & parameters)
  : Material(parameters),
    _wall_boundary_names(getParam<std::vector<BoundaryName>>("walls")),
    _wall_dist(declareProperty<Real>("wall_distance")),
    _compute(false)
{
  const MeshBase & mesh = _subproblem.mesh().getMesh();
  if (!mesh.is_replicated())
    mooseError("WallDistance only supports replicated meshes");
}

void
WallDistance::computeQpProperties()
{
  if (!_compute)
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
          const auto dist = (*node - _current_elem->vertex_average()).norm();
          min_dist = std::min(min_dist, dist);
        }
      }
    }
    _wall_dist[_qp] = min_dist;
    _compute = true;
  }
}
