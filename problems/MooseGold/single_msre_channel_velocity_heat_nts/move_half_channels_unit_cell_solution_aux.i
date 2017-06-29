[Mesh]
  file = msre_squares.msh
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
  [../]
  [./vel_y]
  [../]
  [./vel_z]
  [../]
  [./p]
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
  # [./vel_x_right]
  #   type = SolutionAux
  #   solution = soln_right
  #   variable = vel_x
  #   from_variable = vel_x
  # [../]
  # [./vel_y_right]
  #   type = SolutionAux
  #   solution = soln_right
  #   variable = vel_y
  #   from_variable = vel_y
  # [../]
  # [./vel_z_right]
  #   type = SolutionAux
  #   solution = soln_right
  #   variable = vel_z
  #   from_variable = vel_z
  # [../]
  # [./p_right]
  #   type = SolutionAux
  #   solution = soln_right
  #   variable = p
  #   from_variable = p
  # [../]
  # [./vel_x_top]
  #   type = SolutionAux
  #   solution = soln_top
  #   variable = vel_x
  #   from_variable = vel_x
  # [../]
  # [./vel_y_top]
  #   type = SolutionAux
  #   solution = soln_top
  #   variable = vel_y
  #   from_variable = vel_y
  # [../]
  # [./vel_z_top]
  #   type = SolutionAux
  #   solution = soln_top
  #   variable = vel_z
  #   from_variable = vel_z
  # [../]
  # [./p_top]
  #   type = SolutionAux
  #   solution = soln_top
  #   variable = p
  #   from_variable = p
  # [../]
  # [./vel_x_bottom]
  #   type = SolutionAux
  #   solution = soln_bottom
  #   variable = vel_x
  #   from_variable = vel_x
  # [../]
  # [./vel_y_bottom]
  #   type = SolutionAux
  #   solution = soln_bottom
  #   variable = vel_y
  #   from_variable = vel_y
  # [../]
  # [./vel_z_bottom]
  #   type = SolutionAux
  #   solution = soln_bottom
  #   variable = vel_z
  #   from_variable = vel_z
  # [../]
  # [./p_bottom]
  #   type = SolutionAux
  #   solution = soln_bottom
  #   variable = p
  #   from_variable = p
  # [../]
[]

[UserObjects]
  [./soln_left]
    type = SolutionUserObject
    mesh = single_fuel_channel_MSRE_dimensions_fluid_flow_out.e
    system_variables = 'vel_x vel_y vel_z p'
    timestep = LATEST
    rotation0_vector = '0 0 1'
    rotation0_angle = 90
    translation = '2.032 1.524 0'
    transformation_order = 'rotation0 translation'
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]
