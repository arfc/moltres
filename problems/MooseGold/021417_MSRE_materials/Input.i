flow_velocity=21.7 # cm/s. See MSRE-properties.ods
diri_temp=908
nt_scale=1e13
reactor_height=162.56
global_temperature=temp
# global_temperature=922
sigma_val=6

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 8
  use_exp_form = true
  group_fluxes = 'group1 group2'
#   u_def = 0
#   v_def = 0
#   w_def = ${flow_velocity}
[../]

[Mesh]
  # file = 'msre_22x22_correct_vol_fraction.msh'
  file = msre_cuboid_3x3.msh
[../]

[Nt]
  var_name_base = 'group'
  vacuum_boundaries = 'boundary'
  temp_scaling = 1e0
  nt_ic_function = 'nt_ic_func'
  create_temperature_var = true
  temperature = ${global_temperature}
  # temperature_value = ${global_temperature}
  dg_for_temperature = true
[]

# [PrecursorKernel]
#   var_name_base = pre
#   block = 'fuel'
#   advection_boundaries = 'fuel_top fuel_bottom'
#   family = MONOMIAL
#   order = CONSTANT
# []

[Kernels]
  # Temperature
  [./temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale=${nt_scale}
    block = 'fuel'
  [../]
  # [./temp_source_mod]
  #   type = GammaHeatSource
  #   variable = temp
  #   gamma = .0144 # Cammi .0144
  #   block = 'moder'
  #   average_fission_heat = 'average_fission_heat'
  # [../]
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
  [./temp_diffusion]
    type = MatDiffusion
    prop_name = 'k'
    variable = temp
  [../]
  [./temp_advection_fuel]
    type = ConservativeTemperatureAdvection
    velocity = '0 0 ${flow_velocity}'
    variable = temp
    block = 'fuel'
  [../]
[]

[DGKernels]
  [./temp_advection_fuel]
    block = 'fuel'
    type = DGTemperatureAdvection
    variable = temp
    velocity = '0 0 ${flow_velocity}'
  [../]
  [./temp_diffusion]
    type = DGDiffusion
    variable = temp
    sigma = ${sigma_val}
    epsilon = -1
    diff = 'k'
  [../]
[]

[Materials]
  [./fuel]
    # type = CammiFuel
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = '../property_file_dir/full_core_cuboid_msre_comp_fuel_data_func_of_fuel_temp_'
    prop_names = 'rho k cp'
    prop_values = '2.146e-3 .0553 1967' # Robertson MSRE technical report @ 922 K
    interp_type = 'spline'
    temperature = ${global_temperature}
  [../]
  [./moder]
    # type = CammiModerator
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = '../property_file_dir/full_core_cuboid_msre_comp_mod_data_func_of_mod_temp_'
    prop_names = 'rho k cp'
    prop_values = '1.843e-3 .312 1760' # Cammi 2011 at 908 K
    interp_type = 'spline'
    temperature = ${global_temperature}
  [../]
[]

[BCs]
  [./temp_dirichlet_diffusion_inlet]
    boundary = 'temp_diri_bnd'
    type = DGFunctionDiffusionDirichletBC
    variable = temp
    sigma = ${sigma_val}
    epsilon = -1
    diff = 'k'
    function = 'diri_temp_func'
  [../]
  [./temp_advection_inlet]
    boundary = 'temp_inflow_bnd'
    type = TemperatureInflowBC
    variable = temp
    velocity = '0 0 ${flow_velocity}'
    inlet_conc = ${diri_temp}
  [../]
  [./temp_advection_outlet]
    boundary = 'temp_outflow_bnd'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '0 0 ${flow_velocity}'
  [../]
[]

# [Problem]
#   type = FEProblem
#   coord_type = RZ
# [../]

[Executioner]
  type = Transient
  end_time = 10000
#   num_steps = 1

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-5

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type' # -ksp_gmres_restart'
  petsc_options_value = 'asm	  lu	       1	       preonly'	     # 31'

  nl_max_its = 20
  l_max_its = 10

  dtmin = 1e-5
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-3
    growth_factor = 1.025
    optimal_iterations = 20
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Outputs]
  csv = true
  print_linear_residuals = true
  print_perf_log = true
  [./out]
    type = Exodus
    execute_on = 'initial timestep_end'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

[ICs]
  [./temp_all_ic_func]
    type = FunctionIC
    variable = temp
    function = temp_ic_func
  [../]
[]

[Functions]
  [./temp_ic_func]
    type = ParsedFunction
    value = '${diri_temp}'
  [../]
  [./nt_ic_func]
    type = ParsedFunction
    value = '4/${reactor_height} * y * (1 - y/${reactor_height})'
  [../]
  [./diri_temp_func]
    type = ParsedFunction
    value = '${diri_temp}'
  [../]
[]

[Postprocessors]
  [./group1_current]
    type = IntegralNewVariablePostprocessor
    variable = group1
    outputs = 'csv console'
  [../]
  [./group1_old]
    type = IntegralOldVariablePostprocessor
    variable = group1
    outputs = 'csv console'
  [../]
  [./multiplication]
    type = DivisionPostprocessor
    value1 = group1_current
    value2 = group1_old
    outputs = 'csv console'
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
#   # [./average_fission_heat]
#   #   type = AverageFissionHeat
#   #   nt_scale = ${nt_scale}
#   #   execute_on = 'linear nonlinear'
#   #   outputs = 'csv console'
#   #   block = 'fuel'
#   # [../]
[]
