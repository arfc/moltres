density = .002      # kg cm-3
cp = 3075           # J kg-1 K-1, 6.15 / 2.0e-3
k = .005            # W cm-1 K-1
gamma = 1           # W cm-3 K-1, Volumetric heat transfer coefficient
viscosity = .5      # dynamic viscosity
alpha = 1           # SUPG stabilization parameter
t_alpha = 2e-4      # K-1, Thermal expansion coefficient

# Change "freq" and "dt" values for different perturbation frequencies
freq = 0.8          # Perturbation frequency = 0.8 Hz
dt = 0.00625        # Timestep size = 1 / freq / 200 = 0.00625 s

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
[]

[Mesh]
  [file_mesh]
    type = FileMeshGenerator
    file = '../phase-1/full-coupling_out_ntsApp0_exodus.e'
    use_for_exodus_restart = true
  []
  [corner_node]
    type = ExtraNodesetGenerator
    input = file_mesh
    new_boundary = 'pinned_node'
    coord = '200 200'
  []
[]

[Problem]
  type = FEProblem
[]

[Variables]
  [temp]
    family = LAGRANGE
    order = FIRST
  []
  [vel]
    family = LAGRANGE_VEC
    order = FIRST
  []
  [p]
    family = LAGRANGE
    order = FIRST
  []
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'bottom left right top'
  create_temperature_var = false
  init_nts_from_file = true
  eigenvalue_scaling = 0.9927821802
  ## Use the eigenvalue scaling factor below if running on a 40x40 mesh
  #  eigenvalue_scaling = 0.9926551482
[]

[Precursors]
  [pres]
    var_name_base = pre
    outlet_boundaries = ''
    constant_velocity_values = false
    uvel = velx
    vvel = vely
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    loop_precursors = false
    transient = true
    init_from_file = true
  []
[]

[AuxVariables]
  [velx]
    family = LAGRANGE
    order = FIRST
  []
  [vely]
    family = LAGRANGE
    order = FIRST
  []
  [heat]
    family = MONOMIAL
    order = FIRST
  []
[]

[Kernels]
  [mass]
    type = INSADMass
    variable = p
  []
  [mass_pspg]
    type = INSADMassPSPG
    variable = p
  []
  [momentum_time]
    type = INSADMomentumTimeDerivative
    variable = vel
  []
  [momentum_advection]
    type = INSADMomentumAdvection
    variable = vel
  []
  [momentum_viscous]
    type = INSADMomentumViscous
    variable = vel
  []
  [momentum_pressure]
    type = INSADMomentumPressure
    variable = vel
    pressure = p
  []
  [momentum_supg]
    type = INSADMomentumSUPG
    variable = vel
    velocity = vel
  []
  [buoyancy]
    type = INSADBoussinesqBodyForce
    variable = vel
    # gravity vector, cm s-2
    gravity = '0 -981 0'
    alpha_name = 't_alpha'
    ref_temp = 'temp_ref'
  []
  [gravity]
    type = INSADGravityForce
    variable = vel
    # gravity vector, cm s-2
    gravity = '0 -981 0'
  []

  [temp_time]
    type = INSADHeatConductionTimeDerivative
    variable = temp
  []
  [temp_source]
    type = INSADEnergySource
    variable = temp
    source_variable = heat
  []
  [temp_advection]
    type = INSADEnergyAdvection
    variable = temp
  []
  [temp_conduction]
    type = ADHeatConduction
    variable = temp
    thermal_conductivity = 'k'
  []
  [temp_supg]
    type = INSADEnergySUPG
    variable = temp
    velocity = vel
  []
  [temp_sink]
    type = ConvectiveHeatExchanger
    variable = temp
    htc = ${gamma}
    tref = 900
  []
[]

[AuxKernels]
  [vel_x]
    type = VectorVariableComponentAux
    variable = velx
    vector_variable = vel
    component = 'x'
  []
  [vel_y]
    type = VectorVariableComponentAux
    variable = vely
    vector_variable = vel
    component = 'y'
  []
  [heat_source]
    type = FissionHeatSourceTransientAux
    variable = heat
  []
[]

[UserObjects]
  [initial_th]
    type = SolutionUserObject
    mesh = '../phase-1/full-coupling_exodus.e'
    system_variables = 'velx vely p temp heat'
    timestep = LATEST
    execute_on = INITIAL
  []
[]

[Controls]
  [func_control]
    type = RealFunctionControl
    parameter = 'Kernels/temp_sink/htc'
    function = func_alpha
    execute_on = 'initial timestep_begin'
  []
[]

[Functions]
  [func_alpha]
    type = ParsedFunction
    value = '1 + 0.1 * sin(2*pi*t*${freq})' # Perturbation frequency = 0.8Hz
  []
  [velxf]
    type = SolutionFunction
    from_variable = velx
    solution = initial_th
  []
  [velyf]
    type = SolutionFunction
    from_variable = vely
    solution = initial_th
  []
  [pf]
    type = SolutionFunction
    from_variable = p
    solution = initial_th
  []
  [tempf]
    type = SolutionFunction
    from_variable = temp
    solution = initial_th
  []
  [heatf]
    type = SolutionFunction
    from_variable = heat
    solution = initial_th
  []
[]

[ICs]
  [vel_ic]
    type = VectorFunctionIC
    variable = vel
    function_x = velxf
    function_y = velyf
  []
  [p_ic]
    type = FunctionIC
    variable = p
    function = pf
  []
  [temp_ic]
    type = FunctionIC
    variable = temp
    function = tempf
  []
  [heat_ic]
    type = FunctionIC
    variable = heat
    function = heatf
  []
[]

[BCs]
  [no_slip]
    type = VectorDirichletBC
    variable = vel
    boundary = 'bottom left right'
    values = '0 0 0'
  []
  [lid]
    type = VectorDirichletBC
    variable = vel
    boundary = 'top'
    values = '50 0 0'
  []
  [pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../../property_file_dir/cnrs-benchmark/benchmark_'
    interp_type = 'linear'
    prop_names = 'temp_ref'
    prop_values = '900'
  []
  [ad_mat]
    type = ADGenericConstantMaterial
    prop_names = 'k rho cp mu t_alpha'
    prop_values = '${k} ${density} ${cp} ${viscosity} ${t_alpha}'
  []
  [ins_temp]
    type = INSADStabilized3Eqn
    alpha = ${alpha}
    velocity = vel
    pressure = p
    temperature = temp
  []
[]

[Executioner]
  type = Transient
  scheme = bdf2
  num_steps = 5000

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
  petsc_options_value = 'asm      lu           200                1                NONZERO'
  line_search = 'none'

  automatic_scaling = true
  compute_scaling_once = false
  resid_vs_jac_scaling_param = 0.1
  scaling_group_variables = 'group1 group2 group3 group4 group5 group6; pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8; vel; p; temp'

  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6
  nl_max_its = 25
  l_max_its = 400
  l_tol = 1e-4

  dt = ${dt}
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [memory]
    type = MemoryUsage
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  []
  [powernorm]
    type = ElmIntegTotFissHeatPostprocessor
    execute_on = linear
  []
  [group1norm]
    type = ElementIntegralVariablePostprocessor
    variable = group1
    execute_on = linear
  []
  [group1max]
    type = NodalExtremeValue
    value_type = max
    variable = group1
    execute_on = timestep_end
  []
  [group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
    execute_on = linear
  []
  [group2max]
    type = NodalExtremeValue
    value_type = max
    variable = group2
    execute_on = timestep_end
  []
[]

[VectorPostprocessors]
[]

[Outputs]
  perf_graph = true
  [exodus]
    type = Exodus
    interval = 20
  []
  [csv]
    type = CSV
    append_restart = true
  []
  [checkpoint]
    type = Checkpoint
    num_files = 3
    interval = 20
  []
[]

[Debug]
  show_var_residual_norms = true
[]
