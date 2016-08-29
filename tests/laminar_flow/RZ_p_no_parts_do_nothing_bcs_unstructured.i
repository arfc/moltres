# Model emulating Cammi et. al.
[GlobalParams]
  rho = 1
  mu = 1
  integrate_p_by_parts = false
  gravity = '0 0 0'
  coord_type = RZ
[]

[Mesh]
  # type = GeneratedMesh
  # dim = 2
  # xmin = 0
  # ymin = 0
  # xmax = .5
  # ymax = 1
  # nx = 25
  # ny = 25
  # elem_type = QUAD9
  file = 'gold/2d.msh'
[]

[MeshModifiers]
  [./bottom_left]
    type = AddExtraNodeset
    new_boundary = bottom_left
    coord = '0 0'
  [../]
  [./top_right]
    type = AddExtraNodeset
    new_boundary = top_right
    coord = '0.5 1'
[]


[Problem]
  # coord_type = RZ
[]

[Preconditioning]
  active = 'Newton_FDP'
  [./SMP_PJFNK]
    type = SMP
    full = true

    # Preconditioned JFNK (default)
    solve_type = 'PJFNK'
  [../]
  [./Newton_FDP]
    type = FDP
    full = true
    solve_type = 'NEWTON'
    petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type'
    petsc_options_value = 'lu NONZERO 1.e-10 preonly'
  [../]
  [./JFNK]
    type = SMP
    solve_type = 'JFNK'
  [../]
[]

[Executioner]
  solve_type = JFNK
  # type = Transient
  type = Steady
  # dt = 1e-3
  dtmin = 5e-7
  petsc_options = '-snes_converged_reason -snes_linesearch_monitor -ksp_converged_reason'

  # Basic GMRES/linesearch options only
  # petsc_options_iname = '-ksp_gmres_restart '
  # petsc_options_value = '300                '
  # line_search = 'none'

  nl_rel_tol = 1e-8
  # nl_abs_tol = 1e-7
  # nl_max_its = 20
  l_tol = 1e-3
  l_max_its = 50
  num_steps = 10
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = .01
    growth_factor = 1.2
    optimal_iterations = 25
  [../]

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
  [./p_corner]
    type = DirichletBC
    boundary = top_right
    value = 0
    variable = p
  [../]
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
    # boundary = right
    variable = u
    value = 0
  [../]
  [./v_walls]
    type = DirichletBC
    boundary = right
    # boundary = 'left right'
    variable = v
    value = 0
  [../]
  # [./v_axis]
  #   type = INSSymmetryAxisBC
  #   boundary = left
  #   variable = v
  #   u = u
  #   v = v
  #   p = p
  #   component = 1
  # [../]
[]


[Kernels]
  # mass
  [./mass]
    type = INSMass
    variable = p
    u = u
    v = v
    p = p
    # coord_type = XYZ
  [../]
  [./x_momentum_space]
    type = INSMomentum
    variable = u
    u = u
    v = v
    p = p
    component = 0
  [../]
  # z-momentum, space
  [./y_momentum_space]
    type = INSMomentum
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
    # value = '-16 * (x - 0.25)^2 + 1'
  [../]
[]