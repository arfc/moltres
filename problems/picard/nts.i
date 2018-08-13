flow_velocity=21.7 # cm/s. See MSRE-properties.ods
nt_scale=1e13
ini_temp=922
diri_temp=922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  temperature = temp
  sss2_input = false
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  account_delayed = true
[]

[Mesh]
  file = '2d_lattice_structured.msh'
  # file = '2d_lattice_structured_jac.msh'
[../]

[Problem]
  coord_type = RZ
[]

[Variables]
  [./group1]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1
    scaling = 1e4
  [../]
  [./group2]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1
    scaling = 1e4
  [../]
[]

[AuxVariables]
  [./temp]
  [../]
  [./pre1]
    family = MONOMIAL
    order = CONSTANT
    block = 'fuel'
  [../]
  [./pre2]
    family = MONOMIAL
    order = CONSTANT
    block = 'fuel'
  [../]
  [./pre3]
    family = MONOMIAL
    order = CONSTANT
    block = 'fuel'
  [../]
  [./pre4]
    family = MONOMIAL
    order = CONSTANT
    block = 'fuel'
  [../]
  [./pre5]
    family = MONOMIAL
    order = CONSTANT
    block = 'fuel'
  [../]
  [./pre6]
    family = MONOMIAL
    order = CONSTANT
    block = 'fuel'
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
    block = 'fuel'
  [../]
  [./delayed_group1]
    type = DelayedNeutronSource
    variable = group1
    block = 'fuel'
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
    block = 'fuel'
  [../]
  [./inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  [../]
[]

[BCs]
  [./vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
    variable = group1
  [../]
  [./vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
    variable = group2
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    block = 'fuel'
    prop_names = 'k cp'
    prop_values = '.0553 1967' # Robertson MSRE technical report @ 922 K
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_mod_'
    interp_type = 'spline'
    prop_names = 'k cp'
    prop_values = '.312 1760' # Cammi 2011 at 908 K
    block = 'moder'
  [../]
[]

[Executioner]
  type = Transient
  end_time = 10000

  picard_max_its = 10
  picard_rel_tol = 1e-7
  picard_abs_tol = 1e-10

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type'
  petsc_options_value = 'lu       NONZERO'
  line_search = 'none'
   # petsc_options_iname = '-snes_type'
  # petsc_options_value = 'test'

  nl_max_its = 30
  l_max_its = 100

  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    growth_factor = 1.2
    optimal_iterations = 20
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
    outputs = 'console exodus'
  [../]
  [./group1_old]
    type = IntegralOldVariablePostprocessor
    variable = group1
    outputs = 'console exodus'
  [../]
  [./multiplication]
    type = DivisionPostprocessor
    value1 = group1_current
    value2 = group1_old
    outputs = 'console exodus'
  [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  exodus = true
[]

[Debug]
  show_var_residual_norms = true
[]

[MultiApps]
  [./temp_pres]
    type = TransientMultiApp
    app_type = MoltresApp
    positions = '0 0 0'
    input_files = 'temp.i'
  [../]
[]

[Transfers]
  [./temp_from_sub]
    type = MultiAppCopyTransfer
    direction = from_multiapp
    multi_app = 'temp_pres'
    source_variable = temp
    variable = temp
  [../]
  [./pre1_from_sub]
    type = MultiAppCopyTransfer
    direction = from_multiapp
    multi_app = 'temp_pres'
    source_variable = pre1
    variable = pre1
  [../]
  [./pre2_from_sub]
    type = MultiAppCopyTransfer
    direction = from_multiapp
    multi_app = 'temp_pres'
    source_variable = pre2
    variable = pre2
  [../]
  [./pre3_from_sub]
    type = MultiAppCopyTransfer
    direction = from_multiapp
    multi_app = 'temp_pres'
    source_variable = pre3
    variable = pre3
  [../]
  [./pre4_from_sub]
    type = MultiAppCopyTransfer
    direction = from_multiapp
    multi_app = 'temp_pres'
    source_variable = pre4
    variable = pre4
  [../]
  [./pre5_from_sub]
    type = MultiAppCopyTransfer
    direction = from_multiapp
    multi_app = 'temp_pres'
    source_variable = pre5
    variable = pre5
  [../]
  [./pre6_from_sub]
    type = MultiAppCopyTransfer
    direction = from_multiapp
    multi_app = 'temp_pres'
    source_variable = pre6
    variable = pre6
  [../]
  [./group1_to_sub]
    type = MultiAppCopyTransfer
    direction = to_multiapp
    multi_app = 'temp_pres'
    source_variable = group1
    variable = group1
  [../]
  [./group2_to_sub]
    type = MultiAppCopyTransfer
    direction = to_multiapp
    multi_app = 'temp_pres'
    source_variable = group2
    variable = group2
  [../]
[]
