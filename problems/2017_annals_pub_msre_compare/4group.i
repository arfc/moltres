flow_velocity=21.7 # cm/s. See MSRE-properties.ods
nt_scale=1e13
ini_temp=922
diri_temp=922
gamma_frac=.075
R=73
H=162.56

[GlobalParams]
  num_groups = 4
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4'
  temperature = temp
  sss2_input = true
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  account_delayed = true
  nt_scale = ${nt_scale}
[]

[Mesh]
  file = '2d_lattice_structured.msh'
[]

[Problem]
  coord_type = RZ
[]

[Variables]
  [./temp]
    initial_condition = ${ini_temp}
    scaling = 1e-4
  [../]
[]

[AuxVariables]
  [./power_density]
    order = CONSTANT
    family = MONOMIAL
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
    loop_precursors = false
    family = MONOMIAL
    order = CONSTANT
    # jac_test = true
  [../]
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
  create_temperature_var = false
  scaling = 1e-4
  pre_blocks = 'fuel'
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
    block = 'fuel'
  [../]
  [./temp_source_mod]
    type = GammaHeatSource
    variable = temp
    block = 'moder'
    average_fission_heat = 'average_fission_heat'
    gamma = gamma_func
  [../]
  [./temp_diffusion]
    type = MatDiffusion
    diffusivity = 'k'
    variable = temp
  [../]
  [./temp_advection_fuel]
    type = ConservativeTemperatureAdvection
    velocity = '0 ${flow_velocity} 0'
    variable = temp
    block = 'fuel'
  [../]
[]

[BCs]
  [./temp_diri_cg]
    boundary = 'fuel_bottoms outer_wall'
    type = FlexiblePostprocessorDirichletBC
    postprocessor = coreEndTemp
    offset = -27.8
    variable = temp
  [../]
  # [./temp_diri_cg]
  #   boundary = 'moder_bottoms fuel_bottoms outer_wall'
  #   type = FunctionDirichletBC
  #   function = 'temp_bc_func'
  #   variable = temp
  # [../]
  [./temp_advection_outlet]
    boundary = 'fuel_tops'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '0 ${flow_velocity} 0'
  [../]
[]

[AuxKernels]
  [./fuel]
    block = 'fuel'
    type = FissionHeatSourceTransientAux
    variable = power_density
  [../]
  [./moderator]
    block = 'moder'
    type = ModeratorHeatSourceTransientAux
    average_fission_heat = 'average_fission_heat'
    variable = power_density
    gamma = gamma_func
  [../]
[]

[Functions]
  [./temp_bc_func]
    type = ParsedFunction
    value = '${ini_temp} - (${ini_temp} - ${diri_temp}) * tanh(t/1e-2)'
  [../]
  [./gamma_func]
    type = ParsedFunction
    value = '${gamma_frac} * pi^2 / 4 * cos(pi * x / (2. * ${R})) * sin(pi * y / ${H})'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4g_fuel_rod0_'
    interp_type = 'spline'
    block = 'fuel'
    prop_names = 'k cp'
    prop_values = '.0553 1967' # Robertson MSRE technical report @ 922 K
    peak_power_density = peak_power_density
    controller_gain = 0
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
    property_tables_root = '../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4g_moder_rod0_'
    interp_type = 'spline'
    prop_names = 'k cp'
    prop_values = '.312 1760' # Cammi 2011 at 908 K
    block = 'moder'
    peak_power_density = peak_power_density
    controller_gain = 0
  [../]
  [./rho_moder]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '1.86e-3 * exp(-1.8 * 1.0e-5 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
    block = 'moder'
  [../]
[]

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 6e-6

  solve_type = 'PJFNK'
  line_search = none
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  nl_max_its = 30
  l_max_its = 100

  dtmin = 1e-5
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = limit_k
    dt = 1e-3
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
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
    execute_on = 'linear nonlinear'
    outputs = 'csv console'
    block = 'fuel'
  [../]
  [./coreEndTemp]
    type = SideAverageValue
    variable = temp
    boundary = 'fuel_tops'
    outputs = 'csv console'
    execute_on = 'linear nonlinear'
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
[]

[Debug]
  show_var_residual_norms = true
[]
