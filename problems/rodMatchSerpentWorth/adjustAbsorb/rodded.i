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
  account_delayed = false
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
  eigen = true
[]

[Kernels]
  [./temp_source_fuel]
    type = FissionHeatSource
    variable = temp
    nt_scale=${nt_scale}
    block = 'fuel'
    power = 7.5e6
    tot_fissions = tot_fissions
  [../]
  [./temp_source_mod]
    type = GammaHeatSource
    variable = temp
    gamma = .0144 # Cammi .0144
    block = 'moder'
    average_fission_heat = 'tot_fissions'
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

[BCs]
  [./temp_diri_cg]
    boundary = 'moder_bottoms fuel_bottoms outer_wall'
    type = DirichletBC
    variable = temp
    value = ${diri_temp}
  [../]
  [./temp_advection_outlet]
    boundary = 'fuel_tops'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '0 ${flow_velocity} 0'
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
    property_tables_root = '../../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4gmoder_'
    interp_type = 'spline'
    prop_names = 'k cp'
    prop_values = '.312 1760' # Cammi 2011 at 908 K
    block = 'cRod'
    rodDimension = 'y'
    rodPosition = 23.622
    absorb_factor = 14.22 # how much more absorbing than usual in absorbing region?
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
  type = InversePowerMethod
  max_power_iterations = 50
  xdiff = 'group1diff'

  bx_norm = 'bnorm'
  k0 = 1.5
  pfactor = 1e-2
  l_max_its = 100

  # solve_type = 'PJFNK'
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_factor_shift_type -pc_factor_shift_amount'
  petsc_options_value = 'asm lu NONZERO 1e-10'
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  [./bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    execute_on = linear
  [../]
  [./tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  [../]
  [./group1norm]
    type = ElementIntegralVariablePostprocessor
    variable = group1
    execute_on = linear
  [../]
  [./group1max]
    type = NodalMaxValue
    variable = group1
    execute_on = timestep_end
  [../]
  [./group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
  [./group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
    execute_on = linear
  [../]
  [./group2max]
    type = NodalMaxValue
    variable = group2
    execute_on = timestep_end
  [../]
  [./group2diff]
    type = ElementL2Diff
    variable = group2
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
[]

[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  csv = true
  exodus = true
  execute_on = 'final'
  file_base = 'out'
[]

[Debug]
  show_var_residual_norms = true
[]
