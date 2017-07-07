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
  [./vel_x_left]
    type = SolutionAux
    solution = soln_left
    variable = vel_x
    from_variable = vel_x
  [../]
  [./vel_y_left]
    type = SolutionAux
    solution = soln_left
    variable = vel_y
    from_variable = vel_y
  [../]
  [./vel_z_left]
    type = SolutionAux
    solution = soln_left
    variable = vel_z
    from_variable = vel_z
  [../]
  [./p_left]
    type = SolutionAux
    solution = soln_left
    variable = p
    from_variable = p
  [../]
[]

[UserObjects]
  [./soln_left]
    type = SolutionUserObject
    mesh = single_fuel_channel_MSRE_dimensions_fluid_flow_out.e
    system_variables = 'vel_x vel_y vel_z p'
    timestep = LATEST
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]
