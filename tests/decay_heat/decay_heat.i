flow_velocity=21.7 # cm/s. See MSRE-properties.ods
global_temperature=922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  group_fluxes = '1e9 1e9'
  use_exp_form = false
  decay_heat_fractions = '.01 .01 .01'
  decay_heat_constants = '1 .1 .01'
  temperature = ${global_temperature}
  sss2_input = false
[../]

[Mesh]
  coord_type = RZ
  file = '2d_lattice_structured_smaller.msh'
[../]

[Problem]
  kernel_coverage_check = false
[]

[Variables]
  [./heat1]
    order = CONSTANT
    family = MONOMIAL
    block = 'fuel'
  [../]
  [./heat2]
    order = CONSTANT
    family = MONOMIAL
    block = 'fuel'
  [../]
  [./heat3]
    order = CONSTANT
    family = MONOMIAL
    block = 'fuel'
  [../]
[]

[Kernels]
  [./decay_heat1_source]
    type = HeatPrecursorSource
    variable = heat1
    decay_heat_group_number = 1
    block = 'fuel'
  [../]
  [./decay_heat1_decay]
    type = HeatPrecursorDecay
    variable = heat1
    decay_heat_group_number = 1
    block = 'fuel'
  [../]
  [./decay_heat2_source]
    type = HeatPrecursorSource
    variable = heat2
    decay_heat_group_number = 2
    block = 'fuel'
  [../]
  [./decay_heat2_decay]
    type = HeatPrecursorDecay
    variable = heat2
    decay_heat_group_number = 2
    block = 'fuel'
  [../]
  [./decay_heat3_source]
    type = HeatPrecursorSource
    variable = heat3
    decay_heat_group_number = 3
    block = 'fuel'
  [../]
  [./decay_heat3_decay]
    type = HeatPrecursorDecay
    variable = heat3
    decay_heat_group_number = 3
    block = 'fuel'
  [../]
[]

[DGKernels]
  [./decay_heat1_convection]
    type = DGConvection
    variable = heat1
    velocity = '0 ${flow_velocity} 0'
    block = 'fuel'
  [../]
  [./decay_heat2_convection]
    type = DGConvection
    variable = heat2
    velocity = '0 ${flow_velocity} 0'
    block = 'fuel'
  [../]
  [./decay_heat3_convection]
    type = DGConvection
    variable = heat3
    velocity = '0 ${flow_velocity} 0'
    block = 'fuel'
  [../]
[]

[BCs]
  [./decay_heat1_outflow]
    type = OutflowBC
    variable = heat1
    velocity = '0 ${flow_velocity} 0'
    boundary = 'fuel_tops'
  [../]
  [./decay_heat2_outflow]
    type = OutflowBC
    variable = heat2
    velocity = '0 ${flow_velocity} 0'
    boundary = 'fuel_tops'
  [../]
  [./decay_heat3_outflow]
    type = OutflowBC
    variable = heat3
    velocity = '0 ${flow_velocity} 0'
    boundary = 'fuel_tops'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
  [../]
[]

[Executioner]
  type = Steady

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-5

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'asm      lu           1               preonly       1e-3'

  nl_max_its = 30
  l_max_its = 100
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]


[Outputs]
  perf_graph = true
  print_linear_residuals = true
  csv = true
  [./out]
    type = Exodus
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
