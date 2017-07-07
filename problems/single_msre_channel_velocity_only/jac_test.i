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
  xmax = 1.1
  ymax = 1.1
  zmax = 1.1
  nx = 1
  ny = 1
  nz = 1
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
  # [./mass]
  #   type = INSMass
  #   variable = p
  #   u = vel_x
  #   v = vel_y
  #   w = vel_z
  #   p = p
  # [../]
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

# [BCs]
#   [./x_no_slip]
#     type = DirichletBC
#     variable = vel_x
#     boundary = 'top bottom left right back'
#     value = 0.0
#   [../]
#   [./y_no_slip]
#     type = DirichletBC
#     variable = vel_y
#     boundary = 'left top bottom right back'
#     value = 0.0
#   [../]
#   [./z_no_slip]
#     type = DirichletBC
#     variable = vel_z
#     boundary = 'left top bottom right'
#     value = 0
#   [../]
#   [./z_inlet]
#     type = FunctionDirichletBC
#     variable = vel_z
#     boundary = 'back'
#     function = 'inlet_func'
#   [../]
# []

[Materials]
  [./const]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'rho mu'
    prop_values = '1.1 1.1'
  [../]
[]

[Problem]
  kernel_coverage_check = false
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
  petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type -sub_pc_factor_levels -snes_type'
  petsc_options_value = '300                bjacobi  ilu          4			test'
  petsc_options = '-snes_test_display -snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
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
    value = 'tanh(x/1e-2) * tanh((1 - x)/1e-2) * tanh(y/1e-2) * tanh((1-y)/1e-2)'
  [../]
[]

[ICs]
  # [./p]
  #   type = RandomIC
  #   variable = p
  #   min = 0.5
  #   max = 1.5
  # [../]
  [./vel_x]
    type = RandomIC
    variable = vel_x
    min = 0.5
    max = 1.5
  [../]
  [./vel_y]
    type = RandomIC
    variable = vel_y
    min = 0.5
    max = 1.5
  [../]
  [./vel_z]
    type = RandomIC
    variable = vel_z
    min = 0.5
    max = 1.5
  [../]
  [./p]
    type = ConstantIC
    value = 1
    variable = p
  [../]
  # [./vel_x]
  #   type = ConstantIC
  #   value = 1
  #   variable = vel_x
  # [../]
  # [./vel_y]
  #   type = ConstantIC
  #   value = 1
  #   variable = vel_y
  # [../]
  # [./vel_z]
  #   type = ConstantIC
  #   value = 1
  #   variable = vel_z
  # [../]
[]
