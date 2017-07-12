base_height=136
scale=.99
height=${* ${base_height} ${scale}}

[Mesh]
  file = msre_vol_frac_29x29_h_136.msh
[]

[MeshModifiers]
  [./scale]
    type = Transform
    transform = SCALE
    vector_value = '1 1 ${scale}'
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
  [./group1]
  [../]
  [./group2]
  [../]
  [./pre1]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre2]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre3]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre4]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre5]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./pre6]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./temp]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]

[MultiApps]
  [./sub]
    type = FullSolveMultiApp
    app_type = MoltresApp
    positions = '0 0 0'
    input_files = load_to_new_mesh_sub.i
    execute_on = 'initial'
  [../]
[]

[Transfers]
  [./group1]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = group1
    variable = group1
    execute_on = 'initial'
  [../]
  [./group2]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = group2
    variable = group2
    execute_on = 'initial'
  [../]
  [./pre1]
    type = MultiAppMeshFunctionTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = pre1
    variable = pre1
    execute_on = 'initial'
  [../]
  [./pre2]
    type = MultiAppMeshFunctionTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = pre2
    variable = pre2
    execute_on = 'initial'
  [../]
  [./pre3]
    type = MultiAppMeshFunctionTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = pre3
    variable = pre3
    execute_on = 'initial'
  [../]
  [./pre4]
    type = MultiAppMeshFunctionTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = pre4
    variable = pre4
    execute_on = 'initial'
  [../]
  [./pre5]
    type = MultiAppMeshFunctionTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = pre5
    variable = pre5
    execute_on = 'initial'
  [../]
  [./pre6]
    type = MultiAppMeshFunctionTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = pre6
    variable = pre6
    execute_on = 'initial'
  [../]
  [./temp]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = temp
    variable = temp
    execute_on = 'initial'
  [../]
[]
