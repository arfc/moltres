xmax=1
ymax=1
zmax=1
library_path = '../../../moose/modules/navier_stokes/lib/'

[GlobalParams]
  integrate_p_by_parts = false
  gravity = '0 0 -9.81'
  coord_type = XYZ
[]

[Mesh]
  type = GeneratedMesh
  dim = 3
  xmax = ${xmax}
  ymax = ${ymax}
  zmax = ${zmax}
  nx = 8
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
    solve_type = 'PJFNK'
    petsc_options_iname = '-pc_type'
    petsc_options_value = 'lu'
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.1
  end_time = 100
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
  [./temp]
    initial_condition = 900
    scaling = 1e-4
  [../]
[]

[AuxVariables]
  [./deltaT]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[AuxKernels]
  [./deltaTCalc]
    type =  ConstantDifferenceAux
    variable = deltaT
    compareVar = temp
    constant = 900
  [../]
[]

[BCs]
  [./ux_dirichlet]
    type = DirichletBC
    boundary = 'left right front back bottom top'
    variable = ux
    value = 0
  [../]
  [./uy_dirichlet]
    type = DirichletBC
    boundary = 'left right front back bottom top'
    variable = uy
    value = 0
  [../]
  [./uz_dirichlet]
    type = DirichletBC
    boundary = 'left right front back bottom top'
    variable = uz
    value = 0
  [../]
  [./p_zero]
    type = DirichletBC
    boundary = 'left right front back bottom top'
    variable = p
    value = 0
  [../]
  [./temp_insulate]
    type = NeumannBC
    variable = temp
    value = 0 # no conduction through side walls
    boundary = 'left right front back' # not top
  [../]
  [./coldOnTop]
    type = DirichletBC
    variable = temp
    boundary = top
    value = 800
  [../]
  [./hotOnBottom]
    type = DirichletBC
    variable = temp
    boundary = bottom
    value = 900
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
  [./x_time_deriv]
    type = INSMomentumTimeDerivative
    variable = ux
  [../]
  [./y_time_deriv]
    type = INSMomentumTimeDerivative
    variable = uy
  [../]
  [./z_time_deriv]
    type = INSMomentumTimeDerivative
    variable = uz
  [../]
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
  [./tempTimeDeriv]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
  [./tempAdvectionDiffusion]
    type = INSTemperature
    variable = temp
    u = ux
    v = uy
    w = uz
  [../]
  [./buoyancy_x]
    type = INSBoussinesqBodyForce
    variable = ux
    dT = deltaT
    component = 0
    temperature = temp
  [../]
  [./buoyancy_y]
    type = INSBoussinesqBodyForce
    variable = uy
    dT = deltaT
    component = 1
    temperature = temp
  [../]
  [./buoyancy_z]
    type = INSBoussinesqBodyForce
    variable = uz
    dT = deltaT
    component = 2
    temperature = temp
  [../]
[]


[Materials]
  [./const]
    type = GenericConstantMaterial
    # alpha = coefficient of thermal expansion where rho  = rho0 -alpha * rho0 * delta T
    prop_names = 'mu rho alpha k cp'
    prop_values = '7.0422e-5 .00214 1.32e-4 .0553 1967'

  [../]
[]
