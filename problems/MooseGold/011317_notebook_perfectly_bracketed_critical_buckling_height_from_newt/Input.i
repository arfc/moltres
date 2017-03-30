flow_velocity=147 # Cammi 147 cm/s
inlet_temp=824
initial_outlet_temp=824
nt_scale=1e0
precursor_log_inlet_conc=-17
reactor_height=115 # Cammi 396 cm; critical_buckling_from_newt ~ 115 cm
global_temperature=950

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = true
  group_fluxes = 'group1 group2'
  v_def = ${flow_velocity}
  tau = 1
  transient_simulation = true
  incompressible_flow = false
  # prec_scale = 1e5
[../]

[Mesh]
  file = 'cylinder_structured.msh'
[../]


[Nt]
  var_name_base = 'group'
  vacuum_boundaries = 'fuel_top graphite_top fuel_bottom graphite_bottom'
  temp_scaling = 1e0
  nt_ic_function = 'nt_ic_func'
  create_temperature_var = false
  # temperature = ${global_temperature}
  temperature_value = ${global_temperature}
[]

# [PrecursorKernel]
#   var_name_base = pre
#   block = 'fuel'
#   inlet_boundary = 'fuel_bottom'
#   inlet_boundary_condition = 'DirichletBC'
#   inlet_bc_value = ${precursor_log_inlet_conc}
#   outlet_boundary = 'fuel_top'
#   initial_condition = ${precursor_log_inlet_conc}
#   tau = 1
#   use_source_stabilization = true
#   offset = 24
# []

# [Kernels]
#   # Temperature
#   [./temp_flow_fuel]
#     block = 'fuel'
#     type = MatINSTemperatureRZ
#     variable = temp
#     rho = 'rho'
#     k = 'k'
#     cp = 'cp'
#     uz = ${flow_velocity}
#   [../]
#   [./temp_art_diff_fuel]
#     block = 'fuel'
#     type = ScalarAdvectionArtDiff
#     variable = temp
#     use_exp_form = false
#   [../]
#   [./temp_flow_moder]
#     block = 'moder'
#     type = MatINSTemperatureRZ
#     variable = temp
#     rho = 'rho'
#     k = 'k'
#     cp = 'cp'
#   [../]
#   [./temp_source]
#     type = TransientFissionHeatSource
#     variable = temp
#     nt_scale=${nt_scale}
#   [../]
#   [./temp_time_derivative]
#     type = MatINSTemperatureTimeDerivative
#     variable = temp
#     rho = 'rho'
#     cp = 'cp'
#   [../]
# []

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

# [BCs]
#   [./temp_inlet]
#     boundary = 'fuel_bottom graphite_bottom'
#     type = DirichletBC
#     variable = temp
#     value = ${inlet_temp}
#   [../]
#   [./temp_outlet]
#     boundary = 'fuel_top'
#     type = MatINSTemperatureNoBCBC
#     variable = temp
#     k = 'k'
#   [../]
#   [./temp_art_diff_fuel]
#     boundary = 'fuel_top'
#     type = ScalarAdvectionArtDiffNoBCBC
#     variable = temp
#     use_exp_form = false
#   [../]
# []

[Problem]
  type = FEProblem
  coord_type = RZ
[../]

[Executioner]
  type = Transient
  end_time = 10000

  # line_search = none
  nl_rel_tol = 1e-6
  # nl_abs_tol = 1e-5
  # trans_ss_check = true
  # ss_check_tol = 8e-9

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda -pc_factor_mat_solver_package'
  petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3 mumps'

  nl_max_its = 10
  l_max_its = 10

  dtmin = 1e-5
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-4
    growth_factor = 1.2
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
  [./out]
    type = Exodus
    execute_on = 'initial timestep_end'
  [../]
  [./dof_map]
    type = DOFMap
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

# [ICs]
#   # [./temp_ic]
#   #   type = ConstantIC
#   #   variable = temp
#   #   value = ${inlet_temp}
#   # [../]
#   [./temp_all_ic_func]
#     type = FunctionIC
#     variable = temp
#     function = temp_ic_func
#   [../]
# []

[Functions]
  [./temp_ic_func]
    type = ParsedFunction
    value = '(${initial_outlet_temp} - ${inlet_temp}) / ${reactor_height} * y + ${inlet_temp}'
  [../]
  [./nt_ic_func]
    type = ParsedFunction
    value = '4/${reactor_height} * y * (1 - y/${reactor_height})'
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
  # [./temp_fuel]
  #   type = ElementAverageValue
  #   variable = temp
  #   block = 'fuel'
  #   # execute_on = 'linear nonlinear'
  #   outputs = 'csv console'
  # [../]
  # [./temp_moder]
  #   type = ElementAverageValue
  #   variable = temp
  #   block = 'moder'
  #   # execute_on = 'linear nonlinear'
  #   outputs = 'csv console'
  # [../]
[]