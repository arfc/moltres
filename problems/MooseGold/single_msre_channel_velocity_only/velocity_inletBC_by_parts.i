# This input file tests outflow boundary conditions for the incompressible NS equations.

[GlobalParams]
  gravity = '0 0 0'
  integrate_p_by_parts = true
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  # xmax = 3.048
  # ymax = 1.016
  # zmax = 162.56
  xmax = 1
  ymax = 1
  zmax = 10
  nx = 5
  ny = 5
  nz = 5
  elem_type = HEX27
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
    # prop_values = '2.15e-3  8.28e-5'
    prop_values = '1 2'
  [../]
[]

[Preconditioning]
  [./SMP_PJFNK]
    type = SMP
    full = true
    solve_type = NEWTON
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

[Executioner]
  type = Steady
  # type = Transient
  # dt = 0.1
  # num_steps = 5
  petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type -sub_pc_factor_levels'
  petsc_options_value = '300                bjacobi  ilu          4'
  # petsc_options_iname = '-pc_type -sub_pc_type -sub_pc_factor_levels'
  # petsc_options_value = 'asm	  ilu	       2'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # line_search = none
  # nl_rel_tol = 1e-12
  nl_max_its = 6
  l_tol = 1e-6
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
    # value = '21.73'
    # value = 1
    # value = 'tanh(x/1e-2) * tanh((1 - x)/1e-2) * tanh(y/1e-2) * tanh((1-y)/1e-2)'
    # value = 'tanh(x/1e-3) * tanh((1 - x)/1e-3) * tanh(y/1e-3) * tanh((1-y)/1e-3)'
    value = 'tanh(x/1e-3) * 2'
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
