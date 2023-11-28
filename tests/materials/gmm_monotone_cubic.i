[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  temperature = 922
  sss2_input = false
  account_delayed = false
[]

[Mesh]
  [mesh]
    type = GeneratedMeshGenerator
    dim = 2
  []
[]

[Nt]
  var_name_base = group
  create_temperature_var = false
  eigen = true
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_fuel_'
    interp_type = 'monotone_cubic'
  []
[]

[Executioner]
  type = InversePowerMethod
  xdiff = 'group1diff'
  bx_norm = 'bnorm'
  k0 = 1
  #solve_type = 'PJFNK'
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    execute_on = linear
  []
  [tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  []
  [group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
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
