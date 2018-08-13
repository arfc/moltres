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
  perf_graph = true
  exodus = true
  csv = true
[]
