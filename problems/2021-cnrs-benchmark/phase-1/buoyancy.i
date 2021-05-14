# Step 1.3: Buoyancy input file

density = .002  # kg cm-3
cp = 3075       # J kg-1 K-1, 6.15 / 2.0e-3
k = .005        # W cm-1 K-1
gamma = 1       # W cm-3 K-1, Volumetric heat transfer coefficient
viscosity = .5      # dynamic viscosity, mu = nu * rho, kg cm-1 s-1
alpha = 1           # INS SUPG and PSPG stabilization parameter
t_alpha = 2e-4  # K-1, Thermal expansion coefficient

[GlobalParams]
  num_groups = 6
  num_precursor_groups = 8
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4 group5 group6'
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
  temperature = temp
  sss2_input = true
  account_delayed = true
  integrate_p_by_parts = true
[../]

[Mesh]
  [./square]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 200
    ny = 200
    xmin = 0
    xmax = 200
    ymin = 0
    ymax = 200
    elem_type = QUAD4
  [../]
  [./corner_node]
    type = ExtraNodesetGenerator
    input = square
    new_boundary = 'pinned_node'
    coord = '200 200'
  [../]
[]

[Problem]
  type = FEProblem
[]

[Variables]
  [./temp]
    family = LAGRANGE
    order = FIRST
    scaling = 1e-3
    initial_condition = 900
  [../]
  [./vel]
    family = LAGRANGE_VEC
    order = FIRST
  [../]
  [./p]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'bottom left right top'
  create_temperature_var = false
  eigen = true
  scaling = 1e3
[]

[Precursors]
  [./pres]
    var_name_base = pre
    outlet_boundaries = ''
    constant_velocity_values = false
    uvel = vel_x
    vvel = vel_y
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    loop_precursors = false
    transient = false
    eigen = true
    scaling = 1e3
  [../]
[]

[AuxVariables]
  [./vel_x]
    family = LAGRANGE
    order = FIRST
  [../]
  [./vel_y]
    family = LAGRANGE
    order = FIRST
  [../]
  [./heat]
    family = MONOMIAL
    order = FIRST
  [../]
[]

[Kernels]
  [./mass]
    type = INSADMass
    variable = p
  [../]
  [./mass_pspg]
    type = INSADMassPSPG
    variable = p
  [../]
  [./momentum_advection]
    type = INSADMomentumAdvection
    variable = vel
  [../]
  [./momentum_viscous]
    type = INSADMomentumViscous
    variable = vel
  [../]
  [./momentum_pressure]
    type = INSADMomentumPressure
    variable = vel
    p = p
  [../]
  [./momentum_supg]
    type = INSADMomentumSUPG
    variable = vel
    velocity = vel
  [../]
  [./buoyancy]
    type = INSADBoussinesqBodyForce
    variable = vel
    # gravity vector, cm s-2
    gravity = '0 -981 0'
    alpha_name = 't_alpha'
    ref_temp = 'temp_ref'
  [../]
  [./gravity]
    type = INSADGravityForce
    variable = vel
    # gravity vector, cm s-2
    gravity = '0 -981 0'
  [../]

  [./temp_source]
    type = INSADEnergySource
    variable = temp
    source_variable = heat
  [../]
  [./temp_advection]
    type = INSADEnergyAdvection
    variable = temp
  [../]
  [./temp_conduction]
    type = ADHeatConduction
    variable = temp
    thermal_conductivity = 'k'
  [../]
  [./temp_supg]
    type = INSADEnergySUPG
    variable = temp
    velocity = vel
  [../]
  [./temp_sink]
    type = INSADEnergyAmbientConvection
    variable = temp
    # alpha is the heat transfer coefficient.
    alpha = ${gamma}
    T_ambient = 900
  [../]
[]

[AuxKernels]
  [./vel_x]
    type = VectorVariableComponentAux
    variable = vel_x
    vector_variable = vel
    component = 'x'
  [../]
  [./vel_y]
    type = VectorVariableComponentAux
    variable = vel_y
    vector_variable = vel
    component = 'y'
  [../]
  [./heat_source]
    type = FissionHeatSourceAux
    variable = heat
    tot_fission_heat = powernorm
    power = 1e7
  [../]
[]

[ICs]
  [./vel_ic]
    type = VectorConstantIC
    x_value = 1e-15
    y_value = 1e-15
    variable = vel
  [../]
[]

[BCs]
  [./no_slip]
    type = VectorDirichletBC
    variable = vel
    boundary = 'bottom left right top'
    values = '0 0 0'
  [../]
  [./pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../../property_file_dir/cnrs-benchmark/benchmark_'
    interp_type = 'linear'
    prop_names = 'temp_ref'
    prop_values = '900'
  [../]
  [./ad_mat]
    type = ADGenericConstantMaterial
    prop_names = 'k rho cp mu t_alpha'
    prop_values = '${k} ${density} ${cp} ${viscosity} ${t_alpha}'
  [../]
  [./ins_temp]
    type = INSADStabilized3Eqn
    alpha = ${alpha}
    velocity = vel
    pressure = p
    temperature = temp
  [../]
[]

[Executioner]
  type = NonlinearEigen
  max_power_iterations = 50

  # fission power normalization
  normalization = 'powernorm'
  normal_factor = 1e7           # Watts, 1e9 / 100
  # Tiberga et al. assumes the depth of their 2D domain is 1m.
  # Tiberga et al. domain = 2m x 2m x 1m
  # We divide the total power=1e9W by 100 because our length units are in cm.
  # Our domain = 2m x 2m x 0.01m

  xdiff = 'group1diff'
  bx_norm = 'bnorm'
  k0 = 1.00
  l_max_its = 1000
  nl_max_its = 5000
  nl_abs_tol = 1e-6
  eig_check_tol = 1e-7

  free_power_iterations = 8

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_gasm_overlap -sub_pc_factor_shift_type -pc_gasm_blocks -sub_pc_factor_mat_solver_type'
  petsc_options_value = 'gasm     lu           200                1                NONZERO                   16              superlu_dist'
  line_search = none
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  [./bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    execute_on = linear
  [../]
  [./tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  [../]
  [./powernorm]
    type = ElmIntegTotFissHeatPostprocessor
    execute_on = linear
  [../]
  [./group1norm]
    type = ElementIntegralVariablePostprocessor
    variable = group1
    execute_on = linear
  [../]
  [./group1max]
    type = NodalMaxValue
    variable = group1
    execute_on = timestep_end
  [../]
  [./group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
  [./group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
    execute_on = linear
  [../]
  [./group2max]
    type = NodalMaxValue
    variable = group2
    execute_on = timestep_end
  [../]
  [./group2diff]
    type = ElementL2Diff
    variable = group2
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
[]

[VectorPostprocessors]
  [./vel_aa]
    type = LineValueSampler
    variable = 'vel_x vel_y'
    start_point = '0 100 0'
    end_point = '200 100 0'
    num_points = 201
    sort_by = x
    execute_on = TIMESTEP_END
    outputs = 'csv'
  [../]
  [./vel_bb]
    type = LineValueSampler
    variable = 'vel_x vel_y'
    start_point = '100 0 0'
    end_point = '100 200 0'
    num_points = 201
    sort_by = y
    execute_on = TIMESTEP_END
    outputs = 'csv'
  [../]
  [./temp_aa]
    type = LineValueSampler
    variable = 'temp'
    start_point = '0 100 0'
    end_point = '200 100 0'
    num_points = 201
    sort_by = x
    execute_on = FINAL
    outputs = 'csv'
  [../]
  [./temp_bb]
    type = LineValueSampler
    variable = 'temp'
    start_point = '100 0 0'
    end_point = '100 200 0'
    num_points = 201
    sort_by = y
    execute_on = FINAL
    outputs = 'csv'
  [../]
  [./pre_aa]
    type = LineValueSampler
    variable = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
    start_point = '0.5 100 0'
    end_point = '199.5 100 0'
    num_points = 200
    sort_by = x
    execute_on = FINAL
  [../]
  [./pre_bb]
    type = LineValueSampler
    variable = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
    start_point = '100 0.5 0'
    end_point = '100 199.5 0'
    num_points = 200
    sort_by = y
    execute_on = FINAL
  [../]
  [./pre_elemental]
    type = ElementValueSampler
    variable = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
    sort_by = id
    execute_on = FINAL
  [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [./exodus]
    type = Exodus
  [../]
  [./csv]
    type = CSV
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
