flow_velocity=1
nt_scale = ${/ 1E0 1.6E-13}
ini_temp=1
diri_temp=1

[GlobalParams]
  num_groups = 1
  num_precursor_groups = 1
  use_exp_form = false
  group_fluxes = 'group1'
  temperature = temp
  sss2_input = false
  account_delayed = false
[]

[Mesh]
  type = GeneratedMesh
  nx = 10
  ny = 10
  dim = 2
  xmax = 2.42 # Bessel zero is 2.405
  ymax = 3.149
[]

[Problem]
  coord_type = RZ
[]

[Variables]
  [./group1]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1
    scaling = 1
  [../]
  [./temp]
    initial_condition = ${ini_temp}
    # order = CONSTANT
    # family = MONOMIAL
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
  [./fission_source_group1]
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
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
  [../]
[]

# [DGKernels]
#   [./temp]
#     type = DGConvection
#     variable = temp
#     velocity = '0 ${flow_velocity} 0'
#   [../]
# []

[BCs]
  # [./vacuum_group1]
  #   type = VacuumConcBC
  #   boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
  #   variable = group1
  # [../]
  [./group1]
    type = DirichletBC
    value = 0
    boundary = 'top right bottom'
    variable = 'group1'
  [../]
  [./temp_diri_cg]
    boundary = 'bottom right'
    type = FunctionDirichletBC
    function = 'temp_bc_func'
    variable = temp
  [../]
  # [./temp_advection_inlet]
  #   boundary = 'bottom'
  #   type = TemperatureInflowBC
  #   velocity = '0 ${flow_velocity} 0'
  #   variable = temp
  #   inlet_conc = 1
  # [../]
  [./temp_advection_outlet]
    boundary = 'top'
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
    property_tables_root = '../property_file_dir/dummy_'
    interp_type = 'spline'
    prop_names = 'k cp rho'
    prop_values = '.1 1 1' # Robertson MSRE technical report @ 922 K
  [../]
[]

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'asm      lu           1               preonly       1e-3'
  # petsc_options_iname = '-snes_type'
  # petsc_options_value = 'test'

  nl_max_its = 30
  l_max_its = 100

  dtmin = 1e-5
  # dtmax = 1
  # dt = 1e-3
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-3
    cutback_factor = 0.4
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
    outputs = 'csv console'
  [../]
[]

[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  file_base = ''
  [./exodus]
    type = Exodus
  [../]
  [./csv]
    type = CSV
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
