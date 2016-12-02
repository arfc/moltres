flow_velocity=147 # Cammi 147 cm/s
inlet_temp=824
initial_outlet_temp=1700
nt_scale=1e14
precursor_log_inlet_conc=-17
reactor_height=200 # Cammi 376 cm
fuel_radius=2.08
mod_initial_temp=1500

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 8
  use_exp_form = true
  temperature = temp
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
  scaling = 1e12
  temp_scaling = 1e8
  nt_ic_function = 'nt_ic_func'
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

[Kernels]
  # Temperature
  [./temp_flow_fuel]
    block = 'fuel'
    type = MatINSTemperatureRZ
    variable = temp
    uz = ${flow_velocity}
  [../]
  [./temp_art_diff_fuel]
    block = 'fuel'
    type = ScalarAdvectionArtDiff
    variable = temp
    use_exp_form = false
  [../]
  [./temp_flow_moder]
    block = 'moder'
    type = MatINSTemperatureRZ
    variable = temp
  [../]
  [./temp_source]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale=${nt_scale}
  [../]
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
[]

[Materials]
  [./fuel]
    type = CammiFuel
    block = 'fuel'
    property_tables_root = '../property_file_dir/msr2g_Th_U_two_mat_homogenization_fuel_data_func_of_fuel_temp_'
    prop_names = 'cp'
    prop_values = '1357' # Cammi 2011 at 908 K
  [../]
  [./moder]
    type = CammiModerator
    block = 'moder'
    property_tables_root = '../property_file_dir/msr2g_Th_U_two_mat_homogenization_mod_data_func_of_mod_temp_'
    prop_names = 'rho cp'
    prop_values = '1.843e-3 1760' # Cammi 2011 at 908 K
  [../]
[]

[BCs]
  [./temp_inlet]
    boundary = 'fuel_bottom graphite_bottom'
    type = DirichletBC
    variable = temp
    value = ${inlet_temp}
  [../]
  [./temp_outlet]
    boundary = 'fuel_top'
    type = MatINSTemperatureNoBCBC
    variable = temp
    k = 'k'
  [../]
  [./temp_art_diff_fuel]
    boundary = 'fuel_top'
    type = ScalarAdvectionArtDiffNoBCBC
    variable = temp
    use_exp_form = false
  [../]
[]

[Problem]
  type = FEProblem
  coord_type = RZ
[../]

[Executioner]
  type = Transient
  end_time = 10000

  # line_search = none
  nl_abs_tol = 1e1
  # trans_ss_check = true
  # ss_check_tol = 8e-9

  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type -ksp_gmres_restart -sub_ksp_type -sub_pc_type -pc_asm_overlap -snes_linesearch_minlambda'
  petsc_options_value = 'asm	  31		     preonly	   lu		2		1e-3'
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda -pc_factor_mat_solver_package'
  # petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3 superlu_dist'
  # solve_type = 'PJFNK'
  # petsc_options_iname = '-pc_type -sub_pc_type -snes_linesearch_minlambda -snes_stol'
  # petsc_options_value = 'asm      lu	       1e-3 			  0'

  nl_max_its = 50
  l_max_its = 10

  dtmin = 1e-5
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-4
    growth_factor = 1.05
    optimal_iterations = 20
  [../]
  # [./TimeIntegrator]
  #   type = CrankNicolson
  # [../]
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

[ICs]
  # [./temp_mod_ic]
  #   type = ConstantIC
  #   variable = temp
  #   value = ${mod_initial_temp}
  #   block = 'moder'
  # [../]
  # [./temp_fuel_ic]
  #   type = FunctionIC
  #   block = 'fuel'
  #   function = temp_fuel_ic_func
  #   variable = temp
  # [../]
  [./temp_all_ic]
    type = ConstantIC
    variable = temp
    value = 824
  [../]
[]

[Functions]
  [./temp_ic_func]
    type = ParsedFunction
    value = '(${initial_outlet_temp} - ${inlet_temp}) / ${reactor_height} * y + ${inlet_temp}'
  [../]
  [./temp_fuel_ic_func]
    type = ParsedFunction
    value = '(${mod_initial_temp} - 824) / ${fuel_radius} * x + 824'
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
  [./temp_fuel]
    type = ElementAverageValue
    variable = temp
    block = 'fuel'
    execute_on = 'linear nonlinear'
    # outputs = 'csv console'
  [../]
  [./temp_moder]
    type = ElementAverageValue
    variable = temp
    block = 'moder'
    execute_on = 'linear nonlinear'
    # outputs = 'csv console'
  [../]
[]