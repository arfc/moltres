xmax=.05
ymax=.05

[GlobalParams]
  #integrate_p_by_parts = false
  gravity = '0 -9.81 0'
[]

[Mesh]
  [./generate]
    type = GeneratedMeshGenerator
    dim = 2
    xmax = ${xmax}
    ymax = ${ymax}
    nx = 20
    ny = 20
    elem_type = QUAD9
  []
  [./bottom_left]
    type = ExtraNodesetGenerator
    input = generate
    new_boundary = corner
    coord = '0 0'
  [../]
[]


[Problem]
[]

[Preconditioning]
  [./Newton_SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
  [../]
[]

[Executioner]
  type = Transient
  # Run for 100+ timesteps to reach steady state.
  num_steps = 5
  dt = 0.01
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_test_display'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount'
  petsc_options_value = 'lu	  NONZERO		1e-10'
  # petsc_options_iname = '-snes_type'
  # petsc_options_value = 'test'
   line_search = 'none'
  nl_rel_tol = 1e-12
  nl_max_its = 20
  l_tol = 1e-6
  l_max_its = 500

  dtmin = 1e-5
  #[./TimeStepper]
  #  type = IterationAdaptiveDT
  #  dt = 1e-3
  #  cutback_factor = 0.4
  #  growth_factor = 1.2
  #  optimal_iterations = 20
  #[../]

[]

[Debug]
    show_var_residual_norms = true
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [./out]
    type = Exodus
    execute_on = 'final'
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
  [./p]
    family = LAGRANGE
    order = FIRST
  [../]
  [./temp]
    family = LAGRANGE
    order = SECOND
    initial_condition = 340
    scaling = 1e-4
  [../]
[]


[BCs]
  [./ux_dirichlet]
    type = DirichletBC
    boundary = 'left right bottom top'
    variable = ux
    value = 0
  [../]
  [./uy_dirichlet]
    type = DirichletBC
    boundary = 'left right bottom top'
    variable = uy
    value = 0
  [../]
  [./p_zero]
    type = DirichletBC
    boundary = corner
    variable = p
    value = 0
  [../]
  [./temp_insulate]
    type = NeumannBC
    variable = temp
    value = 0 # no conduction through side walls
    boundary = 'top bottom' # not top
  [../]
  [./coldOnTop]
    type = DirichletBC
    variable = temp
    boundary = left
    value = 300
  [../]
  [./hotOnBottom]
    type = DirichletBC
    variable = temp
    boundary = right
    value = 400
  [../]
[]


[Kernels]
  [./mass]
    type = INSMass
    variable = p
    u = ux
    v = uy
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
  [./x_momentum_space]
    type = INSMomentumLaplaceForm
    variable = ux
    u = ux
    v = uy
    p = p
    component = 0
  [../]
  [./y_momentum_space]
    type = INSMomentumLaplaceForm
    variable = uy
    u = ux
    v = uy
    p = p
    component = 1
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
[]

[Materials]
  [./const]
    type = GenericConstantMaterial
    # alpha = coefficient of thermal expansion where rho  = rho0 -alpha * rho0 * delta T
    prop_names =  'mu        rho    temp_ref alpha   k        cp'
    prop_values = '30.74e-6  .5757  900      2.9e-3  46.38e-3 1054'
  [../]
[]
