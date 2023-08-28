[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  temperature = 900
  sss2_input = true
  account_delayed = true
[]

[Mesh]
  [mesh]
    type = FileMeshGenerator
    file = 'mesh.e'
  []
[]

[Problem]
  coord_type = RZ
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'fuel_bottom mod_bottom right fuel_top mod_top'
  pre_blocks = '0'
  create_temperature_var = false
  eigen = true
[]

[Precursors]
  [pres]
    var_name_base = pre
    family = MONOMIAL
    order = CONSTANT
    block = 0
    outlet_boundaries = 'fuel_top'
    constant_velocity_values = true
    u_def = 0
    v_def = 18.085
    w_def = 0
    nt_exp_form = false
    loop_precursors = false
    transient = false
    eigen = true
  []
[]

[Materials]
  [fuel]
    type = MoltresJsonMaterial
    block = '0'
    base_file = 'xsdata.json'
    material_key = 'fuel'
    interp_type = LINEAR
    prop_names =''
    prop_values = ''
  []
  [graphite]
    type = MoltresJsonMaterial
    block = '1'
    base_file = 'xsdata.json'
    material_key = 'graphite'
    interp_type = LINEAR
    prop_names =''
    prop_values = ''
  []
[]

[Executioner]
  type = InversePowerMethod
  max_power_iterations = 50

  # normalization = 'powernorm'
  # normal_factor = 8e6

  xdiff = 'group1diff'
  bx_norm = 'bnorm'
  k0 = 1.
  l_max_its = 100
  eig_check_tol = 1e-7

  automatic_scaling = true
  compute_scaling_once = false
  resid_vs_jac_scaling_param = 0.1

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       NONZERO               superlu_dist'

  line_search = none
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
  [powernorm]
    type = ElmIntegTotFissHeatPostprocessor
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
  [centerline_flux]
    type = LineValueSampler
    variable = 'group1 group2'
    start_point = '0 0 0'
    end_point = '0 150 0'
    num_points = 151
    sort_by = y
    execute_on = FINAL
  []
  [midplane_flux]
    type = LineValueSampler
    variable = 'group1 group2'
    start_point = '0 75 0'
    end_point = '69.375 75 0'
    num_points = 100
    sort_by = x
    execute_on = FINAL
  []
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [exodus]
    type = Exodus
  []
  [csv]
    type = CSV
  []
[]

[Debug]
  show_var_residual_norms = true
[]
