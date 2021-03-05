# This input file tests outflow boundary conditions for the incompressible NS equations.
width = 3.048
height = 1.016
length = 162.56

[GlobalParams]
  gravity = '0 0 0'
  integrate_p_by_parts = true
[]

[Mesh]
  file = single_channel_msre_dimensions.msh
[]


[Variables]
  [./vel_x]
    order = SECOND
    family = LAGRANGE
    block = 'fuel'
  [../]
  [./vel_y]
    order = SECOND
    family = LAGRANGE
    block = 'fuel'
  [../]
  [./vel_z]
    order = SECOND
    family = LAGRANGE
    block = 'fuel'
  [../]
  [./p]
    order = FIRST
    family = LAGRANGE
    block = 'fuel'
  [../]
  [./temp]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./mass]
    type = INSMass
    variable = p
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    block = 'fuel'
  [../]
  [./x_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_x
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    component = 0
    block = 'fuel'
  [../]
  [./y_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_y
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    component = 1
    block = 'fuel'
  [../]
  [./z_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_z
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    component = 2
    block = 'fuel'
  [../]
  [./temp_fuel_transport]
    type = INSTemperature
    u = vel_x
    v = vel_y
    w = vel_z
    variable = temp
    block = 'fuel'
  [../]
  [./temp_fuel_source]
    type = UserForcingFunction
    variable = temp
    function = fuel_source_function
    block = 'fuel'
  [../]
  [./temp_mod_transport]
    type = MatDiffusion
    diffusivity = 'k'
    variable = temp
    block = 'moderator'
  [../]
  [./temp_mod_source]
    type = UserForcingFunction
    variable = temp
    function = mod_source_function
    block = 'moderator'
  [../]
  # [./x_time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_x
  # [../]
  # [./y_time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_y
  # [../]
  # [./z_time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_z
  # [../]
[]

[BCs]
  # [./pin_p]
  #   type = DirichletBC
  #   variable = p
  #   value = 0
  #   boundary = 'entrance'
  # [../]
  [./x_no_slip]
    type = DirichletBC
    variable = vel_x
    boundary = 'fuel_bottom fuel_side_walls fuel_mod_interface'
    value = 0.0
  [../]
  [./y_no_slip]
    type = DirichletBC
    variable = vel_y
    boundary = 'fuel_bottom fuel_side_walls fuel_mod_interface'
    value = 0.0
  [../]
  [./z_no_slip]
    type = DirichletBC
    variable = vel_z
    boundary = 'fuel_side_walls fuel_mod_interface'
    value = 0
  [../]
  [./z_inlet]
    type = FunctionDirichletBC
    variable = vel_z
    boundary = 'fuel_bottom'
    function = 'inlet_func'
  [../]
  [./temp_inlet]
    boundary = 'fuel_bottom'
    variable = temp
    value = 900
    type = DirichletBC
  [../]
[]

[Materials]
  [./fuel]
    type = GenericConstantMaterial
    block = 'fuel'
    prop_names = 'rho mu k cp'
    prop_values = '2.15e-3  8.28e-5 .0553 1967'
  [../]
  [./moderator]
    type = GenericConstantMaterial
    block = 'moderator'
    prop_names = 'rho k cp'
    prop_values = '1.86e-3 .312 1760'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

[Preconditioning]
  [./SMP_PJFNK]
    type = SMP
    full = true
    solve_type = NEWTON
    ksp_norm = none
  [../]
[]

[Executioner]
  type = Steady
  # type = Transient
  # dt = 5e-5
  # num_steps = 5
  # petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type -sub_pc_factor_levels'
  # petsc_options_value = '300                bjacobi  ilu          4'
  # petsc_options_iname = '-pc_type -sub_pc_type'
  # petsc_options_value = 'asm	  lu'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # line_search = none
  nl_rel_tol = 1e-8
  nl_max_its = 50
  l_max_its = 300
[]

[Outputs]
  perf_graph = true
  [./out]
    type = Exodus
  [../]
[]

[Functions]
  [./inlet_func]
    type = ParsedFunction
    # value = 'tanh(x/1e-2) * tanh((1 - x)/1e-2) * tanh(y/1e-2) * tanh((1-y)/1e-2)'
    value = '21.73 * tanh(x/1e-2) * tanh((${width} - x)/1e-2) * tanh(y/1e-2) * tanh((${height}-y)/1e-2)'
  [../]
  [./fuel_source_function]
    type = ParsedFunction
    value = '10 * sin(pi * z / ${length})'
  [../]
  [./mod_source_function]
    type = ParsedFunction
    value = '.0144 * 4'
  [../]
  [./temp_ic]
    type = ParsedFunction
    value = '900 + 100 / ${length} * z'
  [../]
[]

[ICs]
  [./fuel]
    type = FunctionIC
    variable = temp
    function = temp_ic
  [../]
[]
