[GlobalParams]
  num_groups = 6
  num_precursor_groups = 8
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4 group5 group6'
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
  temperature = temp
  sss2_input = true
  account_delayed = true
[]

[Mesh]
  type = GeneratedMesh
  dim = 2

  nx = 200
  ny = 200
  ## Use a 40-by-40 mesh instead if running on a less capable computer
  #    nx = 40
  #    ny = 40

  xmin = 0
  xmax = 200
  ymin = 0
  ymax = 200
  elem_type = QUAD4
[]

[Problem]
  type = EigenProblem
  bx_norm = bnorm
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'bottom left right top'
  create_temperature_var = false
  eigen = true
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
  []
[]

[AuxVariables]
  [temp]
    family = LAGRANGE
    order = FIRST
    initial_condition = 900
  []
  [ux]
    family = LAGRANGE
    order = FIRST
  []
  [uy]
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
    system_variables = 'vel_x vel_y'
    timestep = LATEST
    execute_on = INITIAL
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

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../../property_file_dir/cnrs-benchmark/benchmark_'
    interp_type = 'spline'
  []
[]

[Executioner]
  type = Eigenvalue

  # fission power normalization
  normalization = 'powernorm'
  normal_factor = 1e7 # Watts, 1e9 / 100
  # Tiberga et al. assumes the depth of their 2D domain is 1m.
  # Tiberga et al. domain = 2m x 2m x 1m
  # We divide the total power=1e9W by 100 because our length units are in cm.
  # Our domain = 2m x 2m x 0.01m

  automatic_scaling = true
  compute_scaling_once = false
  off_diagonals_in_auto_scaling = true
  resid_vs_jac_scaling_param = .1
  scaling_group_variables = 'group1 group2 group3 group4 group5 group6;
                             pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'

  initial_eigenvalue = 1.00
  free_power_iterations = 2

  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'

  ## Alternative PETSc settings if Hypre BoomerAMG is not installed
  #  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
  #  petsc_options_value = 'asm      lu           200                1               NONZERO'

  line_search = none
[]

[Postprocessors]
  [k_eff]
    type = VectorPostprocessorComponent
    index = 0
    vectorpostprocessor = k_vpp
    vector_name = eigen_values_real
  []
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
  [k_vpp]
    type = Eigenvalues
    inverse_eigenvalue = true
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
