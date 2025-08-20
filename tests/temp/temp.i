flow_velocity = 21.7 # cm/s. See MSRE-properties.ods
nt_scale = 1e13
ini_temp = 922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  sss2_input = false
[]

[Mesh]
  coord_type = RZ
  file = '2d_lattice_structured_smaller.msh'
[]

[Variables]
  [temp]
    initial_condition = ${ini_temp}
  []
[]

[Kernels]
  [temp_source_fuel]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale = ${nt_scale}
    block = 'fuel'
    group_fluxes = '1 1'
  []
  [temp_diffusion]
    type = MatDiffusion
    diffusivity = 'k'
    variable = temp
  []
  [temp_advection_fuel]
    type = ConservativeTemperatureAdvection
    velocity_variable = '0 ${flow_velocity} 0'
    variable = temp
    block = 'fuel'
  []
[]

[BCs]
  [temp_diri_cg]
    boundary = 'moder_bottoms fuel_bottoms outer_wall'
    type = FunctionDirichletBC
    function = 'temp_bc_func'
    variable = temp
  []
  [temp_advection_outlet]
    boundary = 'fuel_tops'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '0 ${flow_velocity} 0'
  []
[]

[Functions]
  [temp_bc_func]
    type = ParsedFunction
    expression = '${ini_temp}'
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    temperature = temp
    block = 'fuel'
    prop_names = 'k cp'
    prop_values = '.0553 1967' # Robertson MSRE technical report @ 922 K
  []
  [rho_fuel]
    type = DerivativeParsedMaterial
    property_name = rho
    expression = '2.146e-3 * exp(-1.8 * 1.18e-4 * (temp - 922))'
    coupled_variables = 'temp'
    derivative_order = 1
    block = 'fuel'
  []
  [moder]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_mod_'
    interp_type = 'spline'
    temperature = temp
    prop_names = 'k cp'
    prop_values = '.312 1760' # Cammi 2011 at 908 K
    block = 'moder'
  []
  [rho_moder]
    type = DerivativeParsedMaterial
    property_name = rho
    expression = '1.86e-3 * exp(-1.8 * 1.0e-5 * (temp - 922))'
    coupled_variables = 'temp'
    derivative_order = 1
    block = 'moder'
  []
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
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [temp_fuel]
    type = ElementAverageValue
    variable = temp
    block = 'fuel'
    outputs = 'console'
  []
  [temp_moder]
    type = ElementAverageValue
    variable = temp
    block = 'moder'
    outputs = 'console'
  []
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [out]
    type = Exodus
  []
[]

[Debug]
  show_var_residual_norms = true
[]
