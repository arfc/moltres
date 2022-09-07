# Step 1.2: Power coupling input file
# Requires velocity fields from Step 0.1

density = .002 # kg cm-3
cp = 3075 # J kg-1 K-1, 6.15 / 2.0e-3
k = .005 # W cm-1 K-1
gamma = 1 # W cm-3 K-1, Volumetric heat transfer coefficient
viscosity = .5 # dynamic viscosity, mu = nu * rho, kg cm-1 s-1
alpha = 1 # INS SUPG and PSPG stabilization parameter

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
  type = GeneratedMesh
  dim = 2

  nx = 200
  ny = 200
  ## Use a 40-by-40 mesh instead if running on a desktop/small cluster
  #    nx = 40
  #    ny = 40

  xmin = 0
  xmax = 200
  ymin = 0
  ymax = 200
  elem_type = QUAD4
[]

[Problem]
  type = FEProblem
[]

[Variables]
  [temp]
    family = LAGRANGE
    order = FIRST
    scaling = 1e-3
    initial_condition = 900
  []
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'bottom left right top'
  create_temperature_var = false
  transient = false
  eigen = true
  scaling = 1e3
[]

[Precursors]
  [pres]
    var_name_base = pre
    outlet_boundaries = ''
    constant_velocity_values = false
    uvel = ux
    vvel = uy
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    loop_precursors = false
    transient = false
    eigen = true
    scaling = 1e3
  []
[]

[AuxVariables]
  [ux]
    family = LAGRANGE
    order = FIRST
  []
  [uy]
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
  [heat]
    family = MONOMIAL
    order = FIRST
  []
[]

[UserObjects]
  [velocities]
    type = SolutionUserObject
    mesh = '../phase-0/vel-field_exodus.e'
    system_variables = 'vel_x vel_y p'
    timestep = LATEST
    execute_on = INITIAL
  []
[]

[Kernels]
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
    thermal_conductivity = 'ad_k'
  []
  [temp_supg]
    type = INSADEnergySUPG
    variable = temp
    velocity = vel
  []
  [temp_sink]
    type = INSADEnergyAmbientConvection
    variable = temp
    alpha = ${gamma}
    T_ambient = 900
  []
[]

[AuxKernels]
  [ux]
    type = SolutionAux
    variable = ux
    from_variable = vel_x
    solution = velocities
  []
  [uy]
    type = SolutionAux
    variable = uy
    from_variable = vel_y
    solution = velocities
  []
  [p]
    type = SolutionAux
    variable = p
    from_variable = p
    solution = velocities
  []
  [heat_source]
    type = FissionHeatSourceAux
    variable = heat
    tot_fission_heat = powernorm
    power = 1e7
  []
[]

[Functions]
  [velxf]
    type = SolutionFunction
    from_variable = vel_x
    solution = velocities
  []
  [velyf]
    type = SolutionFunction
    from_variable = vel_y
    solution = velocities
  []
[]

[ICs]
  [vel_ic]
    type = VectorFunctionIC
    variable = vel
    function_x = velxf
    function_y = velyf
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../../property_file_dir/cnrs-benchmark/benchmark_'
    interp_type = 'spline'
    prop_names = 'k rho cp'
    prop_values = '${k} ${density} ${cp}'
  []
  [ad_mat]
    type = ADGenericConstantMaterial
    prop_names = 'ad_k ad_rho ad_cp ad_mu'
    prop_values = '${k} ${density} ${cp} ${viscosity}'
  []
  [ins_mat]
    type = INSADStabilized3Eqn
    k_name = 'ad_k'
    rho_name = 'ad_rho'
    cp_name = 'ad_cp'
    mu_name = 'ad_mu'
    alpha = ${alpha}
    velocity = vel
    pressure = p
    temperature = temp
  []
[]

[Executioner]
  type = NonlinearEigen
  max_power_iterations = 50

  # fission power normalization
  normalization = 'powernorm'
  normal_factor = 1e7 # Watts, 1e9 / 100
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

  free_power_iterations = 4

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_gasm_overlap -sub_pc_factor_shift_type -pc_gasm_blocks -sub_pc_factor_mat_solver_type'
  petsc_options_value = 'gasm     lu           200                1                NONZERO                   16              superlu_dist'

  ## Use the settings below instead if running on a desktop/small cluster
  #  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
  #  petsc_options_value = 'asm      lu           200                1               NONZERO'

  line_search = none
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    execute_on = linear
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
  [group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
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
  [group2diff]
    type = ElementL2Diff
    variable = group2
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  []
[]

[VectorPostprocessors]
  [temp_aa]
    type = LineValueSampler
    variable = 'temp'
    start_point = '0 100 0'
    end_point = '200 100 0'
    num_points = 201
    sort_by = x
    execute_on = FINAL
    outputs = 'csv'
  []
  [temp_bb]
    type = LineValueSampler
    variable = 'temp'
    start_point = '100 0 0'
    end_point = '100 200 0'
    num_points = 201
    sort_by = y
    execute_on = FINAL
    outputs = 'csv'
  []
  [flux_aa]
    type = LineValueSampler
    variable = 'group1 group2 group3 group4 group5 group6'
    start_point = '0 100 0'
    end_point = '200 100 0'
    num_points = 201
    sort_by = x
    execute_on = FINAL
    outputs = 'csv'
  []
  [flux_bb]
    type = LineValueSampler
    variable = 'group1 group2 group3 group4 group5 group6'
    start_point = '100 0 0'
    end_point = '100 200 0'
    num_points = 201
    sort_by = y
    execute_on = FINAL
    outputs = 'csv'
  []
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [exodus]
    type = Exodus
  []
  [csv]
    type = CSV
  []
[]

[Debug]
  show_var_residual_norms = true
[]
