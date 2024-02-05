flow_velocity = 21.7 # cm/s. See MSRE-properties.ods

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  temperature = 922
  sss2_input = true
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  account_delayed = true
[]

[Mesh]
  coord_type = RZ
  file = '2d_lattice_structured.msh'
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
  create_temperature_var = false
  eigen = true
  pre_blocks = 'fuel'
[]

[Precursors]
  [pres]
    var_name_base = pre
    block = 'fuel'
    outlet_boundaries = 'fuel_tops'
    u_def = 0
    v_def = ${flow_velocity}
    w_def = 0
    nt_exp_form = false
    loop_precursors = false
    family = MONOMIAL
    order = CONSTANT
    transient = false
    eigen = true
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_converted_to_serpent/serpent_msre_fuel_'
    interp_type = 'spline'
    block = 'fuel'
  []
  [moder]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_converted_to_serpent/serpent_msre_mod_'
    interp_type = 'spline'
    prop_names = 'k cp'
    prop_values = '.312 1760' # Cammi 2011 at 908 K
    block = 'moder'
  []
[]

[Executioner]
  type = Eigenvalue
  initial_eigenvalue = 1
  l_max_its = 100
  eigen_tol = 1e-7
  normalization = powernorm
  normal_factor = 1e7
  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  line_search = none
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [k_eff]
    type = VectorPostprocessorComponent
    index = 0
    vectorpostprocessor = k_vpp
    vector_name = eigen_values_real
  []
  [fiss_neutrons]
    type = ElmIntegTotFissNtsPostprocessor
    block = 'fuel'
    execute_on = linear
  []
  [powernorm]
    type = ElmIntegTotFissHeatPostprocessor
    execute_on = linear
  []
  [tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  []
  [group1norm]
    type = ElementIntegralVariablePostprocessor
    variable = group1
    execute_on = linear
  []
  [group1max]
    type = NodalExtremeValue
    value_type = max
    variable = group1
    execute_on = timestep_end
  []
  [group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  []
  [group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
    execute_on = linear
  []
  [group2max]
    type = NodalExtremeValue
    value_type = max
    variable = group2
    execute_on = timestep_end
  []
  [group2diff]
    type = ElementL2Diff
    variable = group2
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  []
[]

[VectorPostprocessors]
  [k_vpp]
    type = Eigenvalues
    inverse_eigenvalue = true
  []
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [exodus]
    type = Exodus
    file_base = 'coupled_eigenvalue'
  []
[]
