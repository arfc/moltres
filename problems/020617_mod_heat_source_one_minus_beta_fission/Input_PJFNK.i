flow_velocity=147 # Cammi 147 cm/s
inlet_temp=824
initial_outlet_temp=824
nt_scale=1e16
reactor_height=115 # Cammi 396 cm; critical_buckling_from_newt ~ 115 cm
global_temperature=temp
sigma_val=6

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = true
  group_fluxes = 'group1 group2'
  u_def = 0
  v_def = ${flow_velocity}
  w_def = 0
[../]

[Mesh]
  file = 'cylinder_structured.msh'
[../]

[Nt]
  var_name_base = 'group'
  vacuum_boundaries = 'fuel_top graphite_top fuel_bottom graphite_bottom'
  temp_scaling = 1e0
  nt_ic_function = 'nt_ic_func'
  # create_temperature_var = false
  temperature = ${global_temperature}
  # temperature_value = ${global_temperature}
  dg_for_temperature = true
[]

# [Precursors]
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
  [./temp_source_mod]
    type = GammaHeatSource
    variable = temp
    gamma = .0144 # Cammi .0144
    block = 'moder'
    average_fission_heat = 'average_fission_heat'
  [../]
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
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

[DGKernels]
  [./temp_advection_fuel]
    block = 'fuel'
    type = DGTemperatureAdvection
    variable = temp
    velocity = '0 ${flow_velocity} 0'
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
    type = CammiFuel
    block = 'fuel'
    property_tables_root = '../property_file_dir/newt_fuel_'
    prop_names = 'cp'
    prop_values = '1357' # Cammi 2011 at 908 K
    interp_type = 'spline'
    temperature = ${global_temperature}
  [../]
  [./moder]
    type = CammiModerator
    block = 'moder'
    property_tables_root = '../property_file_dir/newt_mod_'
    prop_names = 'rho cp'
    prop_values = '1.843e-3 1760' # Cammi 2011 at 908 K
    interp_type = 'spline'
    temperature = ${global_temperature}
  [../]
[]

[BCs]
  [./temp_dirichlet_diffusion_inlet]
    boundary = 'fuel_bottom graphite_bottom'
    type = DGFunctionDiffusionDirichletBC
    variable = temp
    sigma = ${sigma_val}
    epsilon = -1
    diff = 'k'
    function = 'inlet_boundary_temp_func'
  [../]
  [./temp_advection_inlet]
    boundary = 'fuel_bottom'
    type = TemperatureInflowBC
    variable = temp
    velocity = '0 ${flow_velocity} 0'
    inlet_conc = 824
  [../]
  [./temp_advection_outlet]
    boundary = 'fuel_top'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '0 ${flow_velocity} 0'
  [../]
[]

[Problem]
  type = FEProblem
  coord_type = RZ
[../]

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-5

  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -ksp_gmres_restart'
  petsc_options_value = 'asm      lu           2               preonly       31'

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
  [./forcing_func]
    type = ParsedFunction
    value = '1000'
  [../]
  [./temp_ic_func]
    type = ParsedFunction
    value = '(${initial_outlet_temp} - ${inlet_temp}) / ${reactor_height} * y + ${inlet_temp}'
  [../]
  [./nt_ic_func]
    type = ParsedFunction
    value = '4/${reactor_height} * y * (1 - y/${reactor_height})'
  [../]
  [./inlet_boundary_temp_func]
    type = ParsedFunction
    value = '${inlet_temp}'
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
  [./average_fission_heat]
    type = AverageFissionHeat
    nt_scale = ${nt_scale}
    execute_on = 'linear nonlinear'
    outputs = 'csv console'
    block = 'fuel'
  [../]
[]
