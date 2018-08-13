xmax=2
ymax=1.2
zmax=0.4

[GlobalParams]
  integrate_p_by_parts = false
  gravity = '0 0 0'
  coord_type = XYZ
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  xmax = ${xmax}
  ymax = ${ymax}
  zmax = ${zmax}
  nx = 5
  ny = 8
  nz = 8
  elem_type = HEX27
[]

[MeshModifiers]
  [./bottom_left]
    type = AddExtraNodeset
    new_boundary = corner
    coord = '0 0 0'
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
  perf_graph = true
  print_linear_residuals = false
  [./out]
    type = Exodus
  []
[]

[Variables]
  [./ux]
    family = LAGRANGE
    order = SECOND
  [../]
  # Velocity in axial direction
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
    # boundary = 'left right front back bottom'
    boundary = 'front back bottom top'
    variable = ux
    value = 0
  [../]
  [./uy_dirichlet]
    type = DirichletBC
    boundary = 'left front back bottom top'
    variable = uy
    value = 0
  [../]
  [./uz_dirichlet]
    type = DirichletBC
    boundary = 'left front back bottom top'
    variable = uz
    value = 0
  [../]
  # [./uz_inlet]
  #   type = FunctionDirichletBC
  #   boundary = left
  #   variable = ux
  #   function = 'inlet_func'
  # [../]
  # [./ux_out]
  #   type = INSMomentumNoBCBCLaplaceForm
  #   boundary = right
  #   variable = ux
  #   u = ux
  #   v = uy
  #   w = uz
  #   p = p
  #   component = 0
  # [../]
  [./uy_out]
    type = INSMomentumNoBCBCLaplaceForm
    boundary = right
    variable = uy
    u = ux
    v = uy
    w = uz
    p = p
    component = 1
  [../]
  [./uz_out]
    type = INSMomentumNoBCBCLaplaceForm
    boundary = right
    variable = uz
    u = ux
    v = uy
    w = uz
    p = p
    component = 2
  [../]
  # [./p_corner]
  #   type = DirichletBC
  #   boundary = corner
  #   variable = p
  #   value = 0
  # [../]
  [./p_left]
    type = DirichletBC
    boundary = left
    variable = p
    value = ${xmax}
  [../]
  [./p_right]
    type = DirichletBC
    boundary = right
    variable = p
    value = 0
  [../]
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
    type = INSMomentumLaplaceForm
    variable = ux
    u = ux
    v = uy
    w = uz
    p = p
    component = 0
  [../]
  [./y_momentum_space]
    type = INSMomentumLaplaceForm
    variable = uy
    u = ux
    v = uy
    w = uz
    p = p
    component = 1
  [../]
  [./z_momentum_space]
    type = INSMomentumLaplaceForm
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
    # value = '-(2/${xmax})^2 * (2/${ymax})^2 * (x - ${xmax}/2)^2 * (y - ${ymax}/2)^2 + 1'
    # value = '16 / (${xmax}^2 * ${ymax}^2) * y * (y - ${ymax}) * x * (x - ${xmax})'
    value = 'sin(pi * z / ${zmax}) * sin(pi * y / ${ymax})'
  [../]
[]

[Materials]
  [./const]
    type = GenericConstantMaterial
    prop_names = 'mu rho'
    prop_values = '1 1'
  [../]
[]
