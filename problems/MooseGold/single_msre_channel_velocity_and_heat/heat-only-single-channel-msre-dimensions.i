# This input file tests outflow boundary conditions for the incompressible NS equations.
width = 3.048
height = 1.016
length = 162.56

[Mesh]
  file = single_channel_msre_dimensions.msh
[]


[Variables]
  [./temp]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./vel_x]
    # order = SECOND
    # family = LAGRANGE
    # block = 'fuel'
  [../]
  [./vel_y]
    # order = SECOND
    # family = LAGRANGE
    # block = 'fuel'
  [../]
  [./vel_z]
    # order = SECOND
    # family = LAGRANGE
    # block = 'fuel'
  [../]
  [./p]
    order = FIRST
    family = LAGRANGE
    block = 'fuel'
  [../]
[]

[Kernels]
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
    D_name = 'k'
    variable = temp
    block = 'moderator'
  [../]
  [./temp_mod_source]
    type = UserForcingFunction
    variable = temp
    function = mod_source_function
    block = 'moderator'
  [../]
[]

[BCs]
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
    prop_names = 'rho k cp'
    prop_values = '2.15e-3 .0553 1967'
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
  # type = Steady
  type = Transient
  dt = 1
  num_steps = 1
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
  print_perf_log = true
  [./out]
    type = Exodus
  [../]
[]

[Functions]
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

[MultiApps]
  [./sub]
    type = TransientMultiApp
    app_type = MoltresApp
    positions = '0 0 0'
    input_files = solution_aux_exodus.i
  [../]
[]

[Transfers]
  [./vel_x]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_x
    variable = vel_x
  [../]
  [./vel_y]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_y
    variable = vel_y
  [../]
  [./vel_z]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_z
    variable = vel_z
  [../]
  [./p]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = p
    variable = p
  [../]
[]
