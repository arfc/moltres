xmax=0.4
ymax=1.2

[GlobalParams]
  rho = 1
  mu = 1
  integrate_p_by_parts = false
  gravity = '0 0 0'
  coord_type = XYZ
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  xmax = ${xmax}
  ymax = ${ymax}
  nx = 10
  ny = 10
  elem_type = QUAD9
[]

[MeshModifiers]
  [./bottom_left]
    type = AddExtraNodeset
    new_boundary = corner
    coord = '0 0'
  [../]
[]


[Problem]
[]

[Preconditioning]
  [./Newton_SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
    petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type'
    petsc_options_value = 'lu NONZERO 1.e-10 preonly'
  [../]
[]

[Executioner]
  type = Steady
  petsc_options = '-snes_converged_reason -snes_linesearch_monitor -ksp_converged_reason'

  nl_rel_tol = 1e-12
[]

[Debug]
        show_var_residual_norms = true
[]

[Outputs]
  print_perf_log = true
  print_linear_residuals = false
  [./out]
    type = Exodus
  []
[]

[Variables]
  [./uz]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[BCs]
  [./uz_dirichlet]
    type = DirichletBC
    boundary = 'left right bottom top'
    variable = uz
    value = 0
  [../]
[]


[Kernels]
  [./diff_uz]
    type = Diffusion
    variable = uz
  [../]
  [./source_uz]
    type = UserForcingFunction
    function = unity_source
    variable = uz
  [../]
[]

[Functions]
  [./unity_source]
    type = ParsedFunction
    value = '1'
  [../]
[]
