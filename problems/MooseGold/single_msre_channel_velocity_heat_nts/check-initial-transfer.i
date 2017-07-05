[Mesh]
  file = single_channel_msre_dimensions.msh
[]


[Variables]
  [./u]
  [../]
[]

[AuxVariables]
  [./vel_x]
    block = 'fuel'
  [../]
  [./vel_y]
    block = 'fuel'
  [../]
  [./vel_z]
    block = 'fuel'
  [../]
  [./p]
    block = 'fuel'
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
    boundary = 'fuel_bottom'
    variable = u
    value = 1
    type = DirichletBC
  [../]
  [./u_outlet]
    boundary = 'fuel_top'
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
  num_steps = 10
  dt = 1
  type = Transient
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
  [./sub]
    type = FullSolveMultiApp
    app_type = MoltresApp
    positions = '0 0 0'
    input_files = solution_aux_exodus.i
    execute_on = 'initial'
  [../]
[]

[Transfers]
  [./vel_x]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_x
    variable = vel_x
    execute_on = 'initial'
  [../]
  [./vel_y]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_y
    variable = vel_y
    execute_on = 'initial'
  [../]
  [./vel_z]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = vel_z
    variable = vel_z
    execute_on = 'initial'
  [../]
  [./p]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub
    source_variable = p
    variable = p
    execute_on = 'initial'
  [../]
[]
