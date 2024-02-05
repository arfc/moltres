[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  temperature = 900
  sss2_input = true
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
    type = MoltresJsonMaterial
    base_file = 'xsdata-900K.json'
    material_key = 'fuel'
    interp_type = 'none'
  []
[]

[Executioner]
  type = Eigenvalue
  initial_eigenvalue = 1
  nl_abs_tol = 1e-12
  solve_type = 'NEWTON'
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
  [k_eff]
    type = VectorPostprocessorComponent
    index = 0
    vectorpostprocessor = k_vpp
    vector_name = eigen_values_real
  []
  [fiss_neutrons]
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

[VectorPostprocessors]
  [k_vpp]
    type = Eigenvalues
    inverse_eigenvalue = true
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
