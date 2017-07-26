flow_velocity=21.7 # cm/s. See MSRE-properties.ods
nt_scale=1e13
ini_temp=922
diri_temp=922

[GlobalParams]
  num_groups = 0
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = ''
  temperature = temp
  sss2_input = false
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  # account_delayed = true
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 600
  xmax = 500
  elem_type = EDGE2
[../]

[Variables]
  [./temp]
    initial_condition = 930 #approx steady outlet of other problem
    scaling = 1e-4
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Precursors]
 [./core]
  var_name_base = pre
  outlet_boundaries = 'right'
  u_def = ${flow_velocity}
  v_def = 0
  w_def = 0
  nt_exp_form = false
  family = MONOMIAL
  order = CONSTANT
 [../]
[]

[Kernels]
  # Temperature
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
  # [./temp_source_fuel]
  #   type = TransientFissionHeatSource
  #   variable = temp
  #   nt_scale=${nt_scale}
  # [../]
  # [./temp_source_mod]
  #   type = GammaHeatSource
  #   variable = temp
  #   gamma = .0144 # Cammi .0144
  #   block = 'moder'
  #   average_fission_heat = 'average_fission_heat'
  # [../]
  # [./temp_diffusion]
  #   type = MatDiffusion
  #   D_name = 'k'
  #   variable = temp
  # [../]
  # [./temp_advection_fuel]
  #   type = ConservativeTemperatureAdvection
  #   velocity = '${flow_velocity} 0 0'
  #   variable = temp
  # [../]
[]

[DGKernels]
  [./temp_adv]
    type = DGTemperatureAdvection
    variable = temp
    velocity = '${flow_velocity} 0 0'
  [../]
[]


[DiracKernels]
  [./heat_exchanger]
    type = DiracHX
    variable = temp
    power = 4e3 # see controls
    point = '250 0 0'
  [../]
[]


[BCs]
  [./fuel_bottoms_looped]
    boundary = 'left'
    type = PostprocessorTemperatureInflowBC
    postprocessor = coreEndTemp
    variable = temp
    uu = ${flow_velocity}
  [../]
  # [./diri]
  #   boundary = 'left'
  #   type = DirichletBC
  #   variable = temp
  #   value = 930
  # [../]
  [./temp_advection_outlet]
    boundary = 'right'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '${flow_velocity} 0 0'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    prop_names = 'k cp'
    prop_values = '.0553 1967' # Robertson MSRE technical report @ 922 K
  [../]
  [./rho_fuel]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '2.146e-3 * exp(-1.8 * 1.18e-4 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
  [../]
[]

[Executioner]
  type = Transient
  end_time = 10000

  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6

  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  line_search = 'none'

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
  [./temp_fuel]
    type = ElementAverageValue
    variable = temp
    outputs = 'exodus console'
  [../]
  [./loopEndTemp]
    type = SideAverageValue
    variable = temp
    boundary = 'right'
  [../]
  [./coreEndTemp]
    type = Receiver
  [../]
[]

[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  [./exodus]
    type = Exodus
    file_base = 'sub'
    execute_on = 'timestep_begin'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

# connect inlet and outlet to multiapp
# [Transfers]
#   [./to_core]
#     type = MultiAppPostprocessorTransfer
#     multi_app = MoltresApp
#     from_postprocessor = loopEndTemp
#     to_postprocessor = inlet_mean_temp
#     direction = to_multiapp
#   [../]
#   [./from_core]
#     type = MultiAppPostprocessorTransfer
#     multi_app = MoltresApp
#     from_postprocessor = coreEndTemp
#     to_postprocessor = coreEndTemp
#     direction = to_multiapp
#   [../]
# []
