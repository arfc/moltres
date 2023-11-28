# Test for ScalarAdvectionArtDiff as a diffusion kernel in a region with
# uniform source

[GlobalParams]
  use_exp_form = false
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 50
  ny = 50
  xmin = 0
  xmax = 50
  ymin = 0
  ymax = 50
[]

[Variables]
  [phi1]
    family = LAGRANGE
    order = FIRST
  []
  [phi2]
    family = LAGRANGE
    order = FIRST
  []
[]

[Kernels]
  # Equivalent to MatDiffusion with D = 0.161363
  [phi1_art_diff]
    type = ScalarAdvectionArtDiff
    variable = phi1
    u_def = 1
    v_def = 1
    w_def = 0
    diffusivity = diff1
  []

  # Equivalent to MatDiffusion with D = 0.706107
  [phi2_art_diff]
    type = ScalarAdvectionArtDiff
    variable = phi2
    u_def = 1
    v_def = 1
    w_def = 0
    diffusivity = diff2
  []

  [phi1_source]
    type = BodyForce
    variable = phi1
    value = 1
  []
  [phi2_source]
    type = BodyForce
    variable = phi2
    value = 1
  []
[]

[BCs]
  [phi1_dirichlet]
    type = DirichletBC
    variable = phi1
    boundary = 'left right top bottom'
    value = 0
  []
  [phi2_dirichlet]
    type = DirichletBC
    variable = phi2
    boundary = 'left right top bottom'
    value = 0
  []
[]

[Materials]
  [square]
    type = GenericConstantMaterial
    prop_names = 'diff1 diff2'
    prop_values = '1 1e-3'
  []
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type'
  petsc_options_value = 'lu       NONZERO'
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  exodus = true
[]

[Debug]
  show_var_residual_norms = true
[]
