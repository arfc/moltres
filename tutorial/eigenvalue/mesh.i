[Mesh]
  [fuel_left]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 0.625
    ymin = 0
    ymax = 150
    nx = 2
    ny = 15
  []
  [fuel_right]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 4.375
    xmax = 5
    ymin = 0
    ymax = 150
    nx = 2
    ny = 15
  []
  [moderator]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0.625
    xmax = 4.375
    ymin = 0
    ymax = 150
    nx = 6
    ny = 15
  []
  [unit_mesh]
    type = StitchedMeshGenerator
    inputs = 'fuel_left moderator fuel_right'
    stitch_boundaries_pairs = 'right left;
                               right left;
                               right left'
  []
  [mod_block]
    type = SubdomainBoundingBoxGenerator
    input = unit_mesh
    block_id = 1
    bottom_left = '0.625 0 0'
    top_right = '4.375 150 0'
  []
  [patterned_mesh]
    type = PatternedMeshGenerator
    inputs = 'mod_block'
    pattern = '0 0 0 0 0 0 0 0 0 0 0 0 0'
    x_width = 5
  []
  [fuel_last]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 65
    xmax = 65.625
    ymin = 0
    ymax = 150
    nx = 2
    ny = 15
  []
  [mod_last]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 65.625
    xmax = 69.375
    ymin = 0
    ymax = 150
    nx = 6
    ny = 15
  []
  [full_mesh]
    type = StitchedMeshGenerator
    inputs = 'patterned_mesh fuel_last mod_last'
    stitch_boundaries_pairs = 'right left;
                               right left;
                               right left'
  []
  [mod_last_block]
    type = SubdomainBoundingBoxGenerator
    input = full_mesh
    block_id = 1
    bottom_left = '65.625 0 0'
    top_right = '69.375 150 0'
  []
  [fuel_top_boundary]
    type = SideSetsAroundSubdomainGenerator
    input = mod_last_block
    block = 0
    new_boundary = 'fuel_top'
    normal = '0 1 0'
  []
  [mod_top_boundary]
    type = SideSetsAroundSubdomainGenerator
    input = fuel_top_boundary
    block = 1
    new_boundary = 'mod_top'
    normal = '0 1 0'
  []
  [fuel_bottom_boundary]
    type = SideSetsAroundSubdomainGenerator
    input = mod_top_boundary
    block = 0
    new_boundary = 'fuel_bottom'
    normal = '0 -1 0'
  []
  [mod_bottom_boundary]
    type = SideSetsAroundSubdomainGenerator
    input = fuel_bottom_boundary
    block = 1
    new_boundary = 'mod_bottom'
    normal = '0 -1 0'
  []
  [delete_boundaries]
    type = BoundaryDeletionGenerator
    input = mod_bottom_boundary
    boundary_names = 'top bottom'
  []
[]
