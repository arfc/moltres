[Mesh]
  file = single_fuel_channel_MSRE_dimensions_fluid_flow_out.e
  parallel_type = replicated
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
[]

[AuxVariables]
  [./vel_x]
    order = SECOND
    family = LAGRANGE
  [../]
  [./vel_y]
    order = SECOND
    family = LAGRANGE
  [../]
  [./vel_z]
    order = SECOND
    family = LAGRANGE
  [../]
  [./p]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[AuxKernels]
  [./vel_x]
    type = SolutionAux
    solution = soln
    variable = vel_x
    from_variable = vel_x
  [../]
  [./vel_y]
    type = SolutionAux
    solution = soln
    variable = vel_y
    from_variable = vel_y
  [../]
  [./vel_z]
    type = SolutionAux
    solution = soln
    variable = vel_z
    from_variable = vel_z
  [../]
  [./p]
    type = SolutionAux
    solution = soln
    variable = p
    from_variable = p
  [../]
[]

[UserObjects]
  [./soln]
    type = SolutionUserObject
    mesh = single_fuel_channel_MSRE_dimensions_fluid_flow_out.e
    system_variables = 'vel_x vel_y vel_z p'
    timestep = LATEST
  [../]
[]

[BCs]
  [./stuff]
    type = DirichletBC
    variable = u
    boundary = '1 2'
    value = 0.0
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 1
  dt = 1
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]
