[GlobalParams]
  # neutronics
  num_groups=6
  num_precursor_groups=8
  use_exp_form = false
  group_fluxes = 'gro1 gro2 gro3 gro4 gro5 gro6'
  temperature = temp
  sss2_input = false
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
  account_delayed = true

  # fluids
  gravity = '0 -9.81 0'
  pspg = true
  convective_term = true
  integrate_p_by_parts = true
  laplace = true
  u = vel_x
  v = vel_y
  p = p
  alpha = 1e-6 # stabilisation parameter
  order = FIRST
  family = LAGRANGE
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  xmax = 1.0
  ymin = 0
  ymax = 1.0
  elem_type = QUAD9
  nx = 20
  ny = 20
[]

[MeshModifiers]
  [./corner_node]
    type = AddExtraNodeset
    new_boundary = 'pinned_node'
    nodes = '0'
  [../]
[]

[Variables]

  # velocities
  [./vel_x]
  [../]

  [./vel_y]
  [../]

  # pressure
  [./p]
  [../]

  # temperature
  [./temp]
    family = LAGRANGE
    order = FIRST
    initial_condition = 900
    scaling = 1e-4
  [../]
[]

# nt -> neutron
[Nt]
  var_name_base = group
  vacuum_boundaries = 'top bottom left right'
  create_temperature_var = false
  eigen = true
  power = 1e9
[]

[Precursors]
  [./preblock]
    var_name_base = pre
    # block = 'fuel'
    constant_velocity_values = true
    u_def = vel_x
    v_def = vel_y
    w_def = 0
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Kernels]
  # mass
  [./mass]
    type = INSMass
    variable = p
  [../]

  # x-momentum, space
  [./x_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_x
    component = 0
  [../]

  # y-momentum, space
  [./y_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_y
    component = 1
  [../]

  [./tempAdvectionDiffusion]
    type = INSTemperature
    variable = temp
    u = ux
    v = uy
  [../]
  [./buoyancy_y]
    type = INSBoussinesqBodyForce
    variable = vel_y
    dT = deltaT
    component = 1
    temperature = temp
  [../]
[]

[BCs]
  [./ux_dirichlet]
    type = DirichletBC
    boundary = 'left right bottom top'
    variable = vel_x
    value = 0
  [../]
  [./uy_dirichlet]
    type = DirichletBC
    boundary = 'left right bottom top'
    variable = vel_y
    value = 0
  [../]
  [./p_pin]
    type=DirichletBC
    variable = p
    value = 0
  [../]
[]

[Materials]
  # alpha = coefficient of thermal expansion where rho  = rho0 -alpha * rho0 * delta T
  # Dr. Aufiero uses nu rather than mu, (kinematic rather than absolute viscosity)
  # likewise, \rho c_p is given rather than separate \rho, c_p
  # accordingly, we let \rho = 1 to obtain equivalent results
  # also, Manu gives a length expansion coefficient. Boussinesq approximations
  # typically use an expansion coefficient with units 1/K, but for mass instead.
  # The relation between the two is alpha = -3 beta, if linearized.
  # Also, had to use Pr = mu * cp / k
  [./const]
    type = GenericConstantMaterial
    prop_names = 'mu     rho alpha   k         cp'
    prop_values = '0.025 1.0 6.0e-4  0.5  6.15e6'
  [../]

  [./fuelneutronicproperties]
    type = GenericMoltresMaterial
    property_tables_root = './groupconstants/'
    interp_type = 'spline'
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    solve_type = 'NEWTON'
  [../]
[]

[Executioner]
  type = Steady
  petsc_options = '-snes_converged_reason -ksp_converged_reason'
  petsc_options_iname = '-pc_type -pc_factor_shift_type'
  petsc_options_value = 'lu NONZERO'
  line_search = 'none'
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-13
  nl_max_its = 6
  l_tol = 1e-6
  l_max_its = 500
[]

[Outputs]
  [./exodus]
    type = Exodus
  [../]
  [./csv]
    type = CSV
  [../]
[]

[Postprocessors]
[]

[AuxVariables]
  [./deltaT]
    type = ConstantDifferenceAux
    variable = deltaT
    comparevar = temp
    constant = 900
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

[Debug]
  show_var_residual_norms = true
[]
