[GlobalParams]
  integrate_p_by_parts = true
  gravity = '0 0 0'
  coord_type = XYZ
[]

[Mesh]
  file = '3d_cone.msh'
[]

[Problem]
[]

[Preconditioning]
  [./Newton_SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
    # petsc_options_iname = '-pc_type -sub_pc_type -sub_ksp_type'
    # petsc_options_value = 'asm            lu           preonly'
    petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -pc_factor_mat_solver_package'
    petsc_options_value = 'lu NONZERO 1.e-10 preonly mumps'
  [../]
[]

[Executioner]
  type = Steady
  # type = Transient
  # num_steps = 10
  # dt = .01
  petsc_options = '-snes_converged_reason -snes_linesearch_monitor -ksp_converged_reason'

  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-12
[]

[Debug]
        show_var_residual_norms = true
[]

[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  [./out]
    type = Exodus
  []
[]

[Variables]
  [./ux]
    family = LAGRANGE
    order = SECOND
  [../]
  [./uy]
    family = LAGRANGE
    order = SECOND
  [../]
  [./uz]
    family = LAGRANGE
    order = SECOND
  [../]
  [./p]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[BCs]
  [./ux_dirichlet]
    type = DirichletBC
    boundary = 'Walls Inlet'
    variable = ux
    value = 0
  [../]
  [./uy_dirichlet]
    type = DirichletBC
    boundary = 'Walls Inlet'
    variable = uy
    value = 0
  [../]
  [./uz_dirichlet]
    type = DirichletBC
    boundary = 'Walls'
    variable = uz
    value = 0
  [../]
  [./uz_inlet]
    type = FunctionDirichletBC
    boundary = 'Inlet'
    variable = uz
    function = 'inlet_func'
  [../]
  [./ux_out]
    type = INSMomentumNoBCBCTractionForm
    boundary = 'Outlet'
    variable = ux
    u = ux
    v = uy
    w = uz
    p = p
    component = 0
  [../]
  [./uy_out]
    type = INSMomentumNoBCBCTractionForm
    boundary = 'Outlet'
    variable = uy
    u = ux
    v = uy
    w = uz
    p = p
    component = 1
  [../]
  [./uz_out]
    type = INSMomentumNoBCBCTractionForm
    boundary = 'Outlet'
    variable = uz
    u = ux
    v = uy
    w = uz
    p = p
    component = 2
  [../]
  [./p_point]
    type = DirichletBC
    boundary = 'PressurePoint'
    variable = p
    value = 0
  [../]

  # [./p_inlet]
  #   type = DirichletBC
  #   boundary = 'Inlet'
  #   variable = p
  #   value = 3
  # [../]
  # [./p_right]
  #   type = DirichletBC
  #   boundary = 'Outlet'
  #   variable = p
  #   value = 0
  # [../]
[]


[Kernels]
  [./mass]
    type = INSMass
    variable = p
    u = ux
    v = uy
    w = uz
    p = p
  [../]
  # [./x_time_deriv]
  #   type = INSMomentumTimeDerivative
  #   variable = ux
  # [../]
  # [./y_time_deriv]
  #   type = INSMomentumTimeDerivative
  #   variable = uy
  # [../]
  # [./z_time_deriv]
  #   type = INSMomentumTimeDerivative
  #   variable = uz
  # [../]
  [./x_momentum_space]
    type = INSMomentumTractionForm
    variable = ux
    u = ux
    v = uy
    w = uz
    p = p
    component = 0
  [../]
  [./y_momentum_space]
    type = INSMomentumTractionForm
    variable = uy
    u = ux
    v = uy
    w = uz
    p = p
    component = 1
  [../]
  [./z_momentum_space]
    type = INSMomentumTractionForm
    variable = uz
    u = ux
    v = uy
    w = uz
    p = p
    component = 2
  [../]
[]

[Functions]
  [./inlet_func]
    type = ParsedFunction
    value = '1 - 4 * (x^2 + y^2)'
  [../]
[]

[Materials]
  [./const]
    type = GenericConstantMaterial
    prop_names = 'mu rho'
    prop_values = '1 1'
  [../]
[]
