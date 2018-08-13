flow_velocity=21.7 # cm/s. See MSRE-properties.ods
nt_scale=1e13
ini_temp=922
diri_temp=922

[GlobalParams]
  num_groups = 4
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4'
  temperature = temp
  sss2_input = true
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  account_delayed = true
[]

[Mesh]
  file = '2d_rodded_lattice.msh'
[../]

[Problem]
  coord_type = RZ
  kernel_coverage_check = false
[]

[Variables]
  [./temp]
    initial_condition = ${ini_temp}
    scaling = 1e-4
  [../]
  [./rodPosition]
    family = SCALAR
    order = FIRST
    initial_condition = 0.0
  [../]
[]

[Precursors]
  [./pres]
    var_name_base = pre
    block = 'fuel'
    outlet_boundaries = 'fuel_tops'
    u_def = 0
    v_def = ${flow_velocity}
    w_def = 0
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    # jac_test = true
  [../]
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall cRod_top cRod_bot'
  create_temperature_var = false
[]

[Kernels]
  # Temperature
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
  [./temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale=${nt_scale}
    block = 'fuel'
  [../]
  [./temp_source_mod]
    type = GammaHeatSource
    variable = temp
    gamma = .0144 # Cammi .0144
    block = 'moder'
    average_fission_heat = 'average_fission_heat'
  [../]
  [./temp_diffusion]
    type = MatDiffusion
    D_name = 'k'
    variable = temp
  [../]
  [./temp_advection_fuel]
    type = ConservativeTemperatureAdvection
    velocity = '0 ${flow_velocity} 0'
    variable = temp
    block = 'fuel'
  [../]
[]

[ScalarKernels]
  [./tdRodPos]
    type = ODETimeDerivative
    variable = rodPosition
  [../]
  [./rodposForce]
    type = ParsedODEKernel
    function = 'rodPosition - 100'
    variable = rodPosition
  [../]
[]

[BCs]
  [./temp_diri_cg]
    boundary = 'moder_bottoms fuel_bottoms outer_wall'
    type = FunctionDirichletBC
    function = 'temp_bc_func'
    variable = temp
  [../]
  [./temp_advection_outlet]
    boundary = 'fuel_tops'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '0 ${flow_velocity} 0'
  [../]
[]

[Functions]
  [./temp_bc_func]
    type = ParsedFunction
    value = '${ini_temp} - (${ini_temp} - ${diri_temp}) * tanh(t/1e-2)'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4gfuel_'
    interp_type = 'spline'
    block = 'fuel'
    prop_names = 'k cp'
    prop_values = '.0553 1967' # Robertson MSRE technical report @ 922 K
  [../]
  [./rho_fuel]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '2.146e-3 * exp(-1.8 * 1.18e-4 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
    block = 'fuel'
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '../../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4gmoder_'
    interp_type = 'spline'
    prop_names = 'k cp'
    prop_values = '.312 1760' # Cammi 2011 at 908 K
    block = 'moder'
  [../]
  [./rho_moder]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '1.86e-3 * exp(-1.8 * 1.0e-5 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
    block = 'moder'
  [../]
  [./cRod]
    type = RoddedMaterial
    property_tables_root = '../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4gmoder_'
    interp_type = 'spline'
    prop_names = 'k cp'
    prop_values = '.312 1760' # Cammi 2011 at 908 K
    block = 'cRod'
    rodDimension = 'y'
    rodPosition = rodPosition
    absorb_factor = 1e6 # how much more absorbing than usual in absorbing region?
  [../]
  [./rho_crod]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '1.86e-3 * exp(-1.8 * 1.0e-5 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
    block = 'cRod'
  [../]

[]

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  line_search = 'none'

  nl_max_its = 30
  l_max_its = 100
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = limit_k
    dt = 0.1
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    # ksp_norm = none
  [../]
[]

[Postprocessors]
  [./group1_current]
    type = IntegralNewVariablePostprocessor
    variable = group1
    outputs = 'console csv'
  [../]
  [./group1_old]
    type = IntegralOldVariablePostprocessor
    variable = group1
    outputs = 'console csv'
  [../]
  [./multiplication]
    type = DivisionPostprocessor
    value1 = group1_current
    value2 = group1_old
    outputs = 'console csv'
  [../]
  [./temp_fuel]
    type = ElementAverageValue
    variable = temp
    block = 'fuel'
    outputs = 'csv console'
  [../]
  [./temp_moder]
    type = ElementAverageValue
    variable = temp
    block = 'moder'
    outputs = 'csv console'
  [../]
  [./average_fission_heat]
     type = AverageFissionHeat
     nt_scale = ${nt_scale}
     execute_on = 'linear nonlinear'
     outputs = 'console'
     block = 'fuel'
  [../]
  [./limit_k]
    type = LimitK
    execute_on = 'timestep_end'
    k_postprocessor = multiplication
    growth_factor = 1.2
    cutback_factor = .4
    k_threshold = 1.5
  [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  csv = true
  exodus = true
  execute_on = 'timestep_end final'
  file_base = 'out'
[]

[Debug]
  show_var_residual_norms = true
[]
