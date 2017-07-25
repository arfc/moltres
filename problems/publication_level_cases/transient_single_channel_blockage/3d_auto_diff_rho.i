flow_velocity=21.7 # cm/s. See MSRE-properties.ods
nt_scale=1e13
ini_temp=922
diri_temp=922
base_height=136
scale=.99
height=${* ${base_height} ${scale}}
width=145
offset=2.5

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  sss2_input = false
  account_delayed = true
  temperature = temp
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
[]

[Mesh]
  file = 3d_ss_out.e
[]

[MeshModifiers]
  # [./scale]
  #   type = Transform
  #   transform = SCALE
  #   vector_value = '1 1 ${scale}'
  # [../]
[]

[Problem]
[]

[Variables]
  [./group1]
    scaling = 1e4
    initial_from_file_var = group1
    initial_from_file_timestep = LATEST
  [../]
  [./group2]
    scaling = 1e4
    initial_from_file_var = group2
    initial_from_file_timestep = LATEST
  [../]
  [./temp]
    scaling = 1e-4
    initial_from_file_var = temp
    initial_from_file_timestep = LATEST
  [../]
[]

[Precursors]
  [./primary_fuel]
    var_name_base = pre
    block = 'fuel blocked_fuel'
    outlet_boundaries = 'fuel_tops'
    u_def = 0
    v_def = 0
    w_def = ${flow_velocity}
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    init_from_file = true
    kernel_block = 'fuel'
  [../]
  [./blocked_fuel]
    var_name_base = pre
    kernel_block = 'blocked_fuel'
    outlet_boundaries = 'blocked_fuel_top'
    u_def = 0
    v_def = 0
    w_def = 0
    nt_exp_form = false
    create_vars = false
    object_suffix = blocked
  [../]
[]

[Kernels]
  # Neutronics
  [./time_group1]
    type = NtTimeDerivative
    variable = group1
    group_number = 1
  [../]
  [./diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  [../]
  [./sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  [../]
  [./fission_source_group1]
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
    block = 'fuel blocked_fuel'
  [../]
  [./delayed_group1]
    type = DelayedNeutronSource
    variable = group1
    block = 'fuel blocked_fuel'
  [../]
  [./inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
  [../]
  [./diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  [../]
  [./sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  [../]
  [./time_group2]
    type = NtTimeDerivative
    variable = group2
    group_number = 2
  [../]
  [./fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
    block = 'fuel blocked_fuel'
  [../]
  [./inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  [../]

  # Temperature
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
  [./temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale=${nt_scale}
    block = 'fuel blocked_fuel'
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
    velocity = '0 0 ${flow_velocity}'
    variable = temp
    block = 'fuel'
  [../]
[]

[BCs]
  [./vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops moder_sides blocked_fuel_top blocked_fuel_bottom'
    variable = group1
  [../]
  [./vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops moder_sides blocked_fuel_top blocked_fuel_bottom'
    variable = group2
  [../]
  [./temp_diri_cg]
    boundary = 'moder_bottoms fuel_bottoms moder_sides'
    type = FunctionDirichletBC
    function = 'temp_bc_func'
    variable = temp
  [../]
  [./temp_advection_outlet]
    boundary = 'fuel_tops'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '0 0 ${flow_velocity}'
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
    type = MsreFuelTwoGrpXSFunctionMaterial
    block = 'fuel blocked_fuel'
    prop_names = 'k cp'
    prop_values = '.0553 1967' # Robertson MSRE technical report @ 922 K
  [../]
  [./rho_fuel]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '2.146e-3 * exp(-1.8 * 1.18e-4 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
    block = 'fuel blocked_fuel'
  [../]
  [./moder]
    type = GraphiteTwoGrpXSFunctionMaterial
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
[]

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  solve_type = 'NEWTON'
  line_search = 'none'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type'
  petsc_options_value = 'lu	  NONZERO		1e-10			preonly'
#   petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
#   petsc_options_value = 'asm      lu           1               preonly       1e-3'
  # petsc_options_iname = '-snes_type'
  # petsc_options_value = 'test'

  nl_max_its = 30
  l_max_its = 200

#   dtmax = 1
  dtmin = 1e-7
  # dt = 1e-3
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 5e-3
    cutback_factor = 0.4
    growth_factor = 1.2
    optimal_iterations = 20
    linear_iteration_ratio = 1000
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    ksp_norm = none
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
    block = 'fuel blocked_fuel'
  [../]
[]

[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  csv = true
  exodus = true
  file_base = temp
[]

[Debug]
  show_var_residual_norms = true
[]
