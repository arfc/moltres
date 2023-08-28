fuel_velocity_max_inlet = 43.4e-2 # double average fluid velocity from MSRE-properties.ods
mu = 8.28e-2 # MSRE-properties.ods
rho = 2.146 # MSRE-properties.ods
fuel_sq_rad = 39.94

[GlobalParams]
  gravity = '0 0 0'
  integrate_p_by_parts = true
[]

[Mesh]
  # file = 3d_single_unit_msre_vol_fraction.msh
  file = 3d_only_single_fuel_unit.msh
[]

[Problem]
  # kernel_coverage_check = false
[]

[Variables]
  [vel_x]
    order = SECOND
    family = LAGRANGE
    block = 'fuel'
  []
  [vel_y]
    order = SECOND
    family = LAGRANGE
    block = 'fuel'
  []
  [vel_z]
    order = SECOND
    family = LAGRANGE
    block = 'fuel'
  []
  [p]
    order = FIRST
    family = LAGRANGE
    block = 'fuel'
  []
[]

[Kernels]
  # [./x-time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_x
  # [../]
  # [./y-time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_y
  # [../]
  # [./z-time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_z
  # [../]
  [mass]
    type = INSMass
    variable = p
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
  []
  [x_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_x
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    component = 0
  []
  [y_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_y
    u = vel_x
    v = vel_y
    p = p
    component = 1
  []
  [z_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_z
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    component = 2
  []
[]

[BCs]
  [z_no_slip]
    type = DirichletBC
    variable = vel_z
    boundary = 'fuel_sides'
    value = 0.0
  []
  [y_no_slip]
    type = DirichletBC
    variable = vel_y
    boundary = 'fuel_sides fuel_bottoms'
    value = 0.0
  []
  [x_no_slip]
    type = DirichletBC
    variable = vel_x
    boundary = 'fuel_sides fuel_bottoms'
    value = 0.0
  []
  [z_inlet]
    type = FunctionDirichletBC
    variable = vel_z
    boundary = 'fuel_bottoms'
    function = 'inlet_func'
  []
[]

[Materials]
  [const]
    type = GenericConstantMaterial
    # block = 'fuel moder'
    block = 'fuel'
    prop_names = 'rho mu'
    prop_values = '${rho}  ${mu}'
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
    solve_type = PJFNK
  []
[]

[Executioner]
  type = Steady
  # type = Transient
  # end_time = 8
  # dt = .8
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type -sub_pc_factor_levels'
  petsc_options_value = '300                bjacobi  ilu          1'
  # line_search = none
  nl_rel_tol = 1e-6
  # nl_max_its = 6
  l_tol = 1e-5
  l_max_its = 300
[]

[Outputs]
  [out]
    type = Exodus
  []
[]

[Functions]
  [inlet_func]
    type = ParsedFunction
    # expression = '${fuel_velocity_max_inlet} * (${fuel_sq_rad}^2 - x^2) * (${fuel_sq_rad}^2 - y^2) / ${fuel_sq_rad}^4'
    expression = '${fuel_velocity_max_inlet} * (1 - cosh(pi * y / (2 * ${fuel_sq_rad})) / cosh(pi / 2)) * cos(pi * x / (2 * ${fuel_sq_rad}))'
    # expression = '${fuel_velocity_max_inlet} * cos(pi * x / (2 * ${fuel_sq_rad})) * cos(pi * y / (2 * ${fuel_sq_rad}))'
  []
[]
