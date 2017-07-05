[Mesh]
  type = GeneratedMesh
  nx = 20
  ny = 20
  dim = 2
  xmin = -0.5
  ymin = -0.5
  xmax = 0.5
  ymax = .5
[]

[MeshModifiers]
  [./box]
    type = SubdomainBoundingBox
    # bottom_left = '0 -.25 0'
    # top_right = '.5 .25 0'
    bottom_left = '-.25 -.25 0'
    top_right = '.25 .25 0'
    block_id = 1
    block_name = box
  [../]
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
[]

[AuxVariables]
  [./v]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[AuxKernels]
  [./v]
    type = SolutionAux
    solution = soln
    variable = v
    from_variable = u
    block = box
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    boundary = left
    variable = u
    value = 1
  [../]
  [./right]
    type = DirichletBC
    boundary = right
    variable = u
    value = 0
  [../]
[]

[UserObjects]
  [./soln]
    type = SolutionUserObject
    mesh = solution_out.e
    system_variables = 'u'
    timestep = LATEST
    rotation0_vector = '0 0 1'
    rotation0_angle = 90
    # translation = '.25 0 0'
    # transformation_order = 'rotation0 translation'
    transformation_order = 'rotation0'
    # transformation_order = 'translation'
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]
