[GlobalParams]
  rho = 1
  mu = 1
  integrate_p_by_parts = false
  gravity = '0 0 0'
  coord_type = RZ
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  ymin = 0
  xmax = .5
  ymax = 1
  nx = 25
  ny = 25
  elem_type = QUAD9
[]

[MeshModifiers]
  [./bottom_left]
    type = AddExtraNodeset
    new_boundary = top_right
    coord = '0.5 1'
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
  l_tol = 1e-3
  l_max_its = 50
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
  # Velocity in radial direction
  [./u]
    family = LAGRANGE
    order = SECOND
  [../]
  # Velocity in axial direction
  [./v]
    family = LAGRANGE
    order = SECOND
  [../]
  [./p]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[BCs]
  [./u_in]
    type = DirichletBC
    boundary = bottom
    variable = u
    value = 0 # m/s
  [../]
  [./v_in]
    type = FunctionDirichletBC
    boundary = bottom
    variable = v
    function = 'inlet_func'
  [../]
  [./u_out]
    type = INSMomentumNoBCBC
    boundary = top
    variable = u
    u = u
    v = v
    p = p
    component = 0
  [../]
  [./v_out]
    type = INSMomentumNoBCBC
    boundary = top
    variable = v
    u = u
    v = v
    p = p
    component = 1
  [../]
  [./u_axis_and_walls]
    type = DirichletBC
    boundary = 'left right'
    variable = u
    value = 0
  [../]
  [./v_walls]
    type = DirichletBC
    boundary = right
    variable = v
    value = 0
  [../]
  [./p_corner]
    type = DirichletBC
    variable = p
    value = 0
    boundary = top_right
  [../]
[]


[Kernels]
  # mass
  [./mass]
    type = INSMassRZ
    variable = p
    u = u
    v = v
    p = p
  [../]
  [./u_momentum_space]
    type = INSMomentumRZ
    variable = u
    u = u
    v = v
    p = p
    component = 0
  [../]
  [./v_momentum_space]
    type = INSMomentumRZ
    variable = v
    u = u
    v = v
    p = p
    component = 1
  [../]
[]

[Functions]
  [./inlet_func]
    type = ParsedFunction
    value = '-4 * x^2 + 1'
  [../]
[]
