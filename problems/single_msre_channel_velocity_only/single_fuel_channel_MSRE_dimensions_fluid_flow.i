# This input file tests outflow boundary conditions for the incompressible NS equations.
width = 3.048
height = 1.016

[GlobalParams]
  gravity = '0 0 0'
  integrate_p_by_parts = true
[]

[Mesh]
  file = single_fuel_channel_msre_dimensions_bump.msh
[]


[Variables]
  [./vel_x]
    order = SECOND
    family = LAGRANGE
  [../]
  [./vel_y]
    order = SECOND
    family = LAGRANGE
  [../]
  [./vel_z]
    order = SECOND
    family = LAGRANGE
  [../]
  [./p]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./mass]
    type = INSMass
    variable = p
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
  [../]
  [./x_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_x
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    component = 0
  [../]
  [./y_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_y
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    component = 1
  [../]
  [./z_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_z
    u = vel_x
    v = vel_y
    w = vel_z
    p = p
    component = 2
  [../]
  # [./x_time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_x
  # [../]
  # [./y_time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_y
  # [../]
  # [./z_time]
  #   type = INSMomentumTimeDerivative
  #   variable = vel_z
  # [../]
[]

[BCs]
  # [./pin_p]
  #   type = DirichletBC
  #   variable = p
  #   value = 0
  #   boundary = 'entrance'
  # [../]
  [./x_no_slip]
    type = DirichletBC
    variable = vel_x
    boundary = 'top bottom left right back'
    value = 0.0
  [../]
  [./y_no_slip]
    type = DirichletBC
    variable = vel_y
    boundary = 'left top bottom right back'
    value = 0.0
  [../]
  [./z_no_slip]
    type = DirichletBC
    variable = vel_z
    boundary = 'left top bottom right'
    value = 0
  [../]
  [./z_inlet]
    type = FunctionDirichletBC
    variable = vel_z
    boundary = 'back'
    function = 'inlet_func'
  [../]
[]

[Materials]
  [./const]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'rho mu'
    prop_values = '2.15e-3  8.28e-5'
    # prop_values = '1 2'
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
  # type = Transient
  # dt = 5e-5
  # num_steps = 5
  # petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type -sub_pc_factor_levels'
  # petsc_options_value = '300                bjacobi  ilu          4'
  # petsc_options_iname = '-pc_type -sub_pc_type'
  # petsc_options_value = 'asm	  lu'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # line_search = none
  nl_rel_tol = 1e-8
  nl_max_its = 50
  l_max_its = 300
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]

[Functions]
  [./inlet_func]
    type = ParsedFunction
    # value = 'tanh(x/1e-2) * tanh((1 - x)/1e-2) * tanh(y/1e-2) * tanh((1-y)/1e-2)'
    value = '21.73 * tanh(x/1e-2) * tanh((${width} - x)/1e-2) * tanh(y/1e-2) * tanh((${height}-y)/1e-2)'
  [../]
[]

# [Adaptivity]
#   marker = errorfrac_vel_z
#   max_h_level = 2
#   steps = 2
#   [./Indicators]
#     [./error_vel_z]
#       type = GradientJumpIndicator
#       variable = vel_z
#       outputs = none
#     [../]
#   [../]
#   [./Markers]
#     [./errorfrac_vel_z]
#       type = ErrorFractionMarker
#       refine = 0.8
#       coarsen = 0.1
#       indicator = error_vel_z
#       outputs = none
#     [../]
#   [../]
# []
