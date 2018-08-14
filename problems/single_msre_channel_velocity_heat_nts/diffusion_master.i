[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
[]


[Variables]
  [./u]
  [../]
[]

[AuxVariables]
  [./from_sub]
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
    boundary = 'left'
    variable = u
    value = 1
    type = DirichletBC
  [../]
  [./u_outlet]
    boundary = 'right'
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
  perf_graph = true
  exodus = true
  csv = true
[]

[MultiApps]
  [./sub_horizontal]
    type = FullSolveMultiApp
    app_type = MoltresApp
    positions = '-0.5 -0.5 0'
    input_files = diffusion_sub.i
    execute_on = 'initial'
  [../]
[]

[Transfers]
  [./vel_x_horizontal]
    type = MultiAppNearestNodeTransfer
    direction = from_multiapp
    multi_app = sub_horizontal
    source_variable = u
    variable = from_sub
    execute_on = 'initial'
  [../]
[]
