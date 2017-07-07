fuel_width=3.048
fuel_height=1.016
cell_width=5.08

[Mesh]
  file = msre_squares.msh
[]


[Variables]
  [./u]
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
  [./u_diff]
    type = Diffusion
    variable = u
  [../]
[]


[BCs]
  [./u_inlet]
    boundary = 'fuel_bottoms'
    variable = u
    value = 1
    type = DirichletBC
  [../]
  [./u_outlet]
    boundary = 'fuel_tops'
    variable = u
    value = 0
    type = DirichletBC
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
  # num_steps = 1
  # dt = 1
  type = Steady
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3'

  nl_max_its = 30
  l_max_its = 200
[]

[Outputs]
  print_perf_log = true
  exodus = true
  csv = true
[]

[MultiApps]
  [./sub_horizontal]
    type = FullSolveMultiApp
    app_type = MoltresApp
    positions = '-1.524 2.032 0 -1.524 -3.048 0'
    input_files = solution_aux_exodus.i
    execute_on = 'initial'
  [../]
  [./sub_vertical]
    type = FullSolveMultiApp
    app_type = MoltresApp
    positions = '2.032 1.524 0 -3.048 1.524 0'
    input_files = rotated_solution_aux_exodus.i
    execute_on = 'initial'
  [../]
[]

[Transfers]
  [./vel_x_horizontal]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_horizontal
    source_variable = vel_x
    variable = vel_x
    execute_on = 'initial'
  [../]
  [./vel_y_horiztonal]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_horizontal
    source_variable = vel_y
    variable = vel_y
    execute_on = 'initial'
  [../]
  [./vel_z_horizontal]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_horizontal
    source_variable = vel_z
    variable = vel_z
    execute_on = 'initial'
  [../]
  [./p_horizontal]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_horizontal
    source_variable = p
    variable = p
    execute_on = 'initial'
  [../]
  [./vel_x_vertical]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_vertical
    source_variable = vel_x
    variable = vel_x
    execute_on = 'initial'
  [../]
  [./vel_y_vertical]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_vertical
    source_variable = vel_y
    variable = vel_y
    execute_on = 'initial'
  [../]
  [./vel_z_vertical]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_vertical
    source_variable = vel_z
    variable = vel_z
    execute_on = 'initial'
  [../]
  [./p_vertical]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_vertical
    source_variable = p
    variable = p
    execute_on = 'initial'
  [../]
[]
