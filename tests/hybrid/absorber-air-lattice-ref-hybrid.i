[GlobalParams]
  num_groups = 8
  num_precursor_groups = 6
  group_fluxes = 'group1 group2 group3 group4 group5 group6 group7 group8'
  temperature = 900
  sss2_input = true
  account_delayed = false
  use_exp_form = false
  set_diffcoef_limit = true
  search_value_conflicts = false
  bottom_left = '0 0 0'
  top_right = '10 0 0'
[]

[Problem]
  type = EigenProblem
  bx_norm = bnorm
[]

[Mesh]
  second_order = true
  [cmg]
    type = CartesianMeshGenerator
    dim = 1
    dx = '0.4 0.1 1 1.5625 3.875 1.125 3.875 1.125 3.875 1.125 3.875 1.125 3.875 1.125 3.875 1.125 3.875 1.125 3.875 3.0625 5'
    ix = '4 2 4 6 10 4 10 4 10 4 10 4 10 4 10 4 10 4 10 12 32'
    subdomain_id = '0 0 1 2 3 2 3 2 3 2 3 2 3 2 3 2 3 2 3 2 4'
  []
  [fuel]
    type = SubdomainBoundingBoxGenerator
    input = cmg
    restricted_subdomains = 2
    block_id = 5
    bottom_left = '0 0 0'
    top_right = '5 0 0'
  []
  [graphite]
    type = SubdomainBoundingBoxGenerator
    input = fuel
    restricted_subdomains = 3
    block_id = 6
    bottom_left = '0 0 0'
    top_right = '7.5 0 0'
  []
  [fuel_adaptive]
    type = SubdomainBoundingBoxGenerator
    input = graphite
    restricted_subdomains = 2
    block_id = 7
    bottom_left = '5 0 0'
    top_right = '10 0 0'
  []
[]

[AuxVariables]
  [drift1]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift2]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift3]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift4]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift5]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift6]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift7]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
  [drift8]
    order = SECOND
    family = MONOMIAL
    components = 3
  []
[]

[Nt]
  family = LAGRANGE
  order = SECOND
  var_name_base = group
  vacuum_boundaries = 'right'
  fission_blocks = '2 5 7'
  create_temperature_var = false
  eigen = true
[]

[Kernels]
  [group1_drift]
    type = GroupDrift
    variable = group1
    block = '0 1 5 6'
    group_drift_var = drift1
  []
  [group2_drift]
    type = GroupDrift
    variable = group2
    block = '0 1 5 6'
    group_drift_var = drift2
  []
  [group3_drift]
    type = GroupDrift
    variable = group3
    block = '0 1 5 6'
    group_drift_var = drift3
  []
  [group4_drift]
    type = GroupDrift
    variable = group4
    block = '0 1 5 6'
    group_drift_var = drift4
  []
  [group5_drift]
    type = GroupDrift
    variable = group5
    block = '0 1 5 6'
    group_drift_var = drift5
  []
  [group6_drift]
    type = GroupDrift
    variable = group6
    block = '0 1 5 6'
    group_drift_var = drift6
  []
  [group7_drift]
    type = GroupDrift
    variable = group7
    block = '0 1 5 6'
    group_drift_var = drift7
  []
  [group8_drift]
    type = GroupDrift
    variable = group8
    block = '0 1 5 6'
    group_drift_var = drift8
  []
  [group1_drift_adaptive]
    type = GroupDrift
    variable = group1
    block = '7'
    group_drift_var = drift1
    adaptive = true
  []
  [group2_drift_adaptive]
    type = GroupDrift
    variable = group2
    block = '7'
    group_drift_var = drift2
    adaptive = true
  []
  [group3_drift_adaptive]
    type = GroupDrift
    variable = group3
    block = '7'
    group_drift_var = drift3
    adaptive = true
  []
  [group4_drift_adaptive]
    type = GroupDrift
    variable = group4
    block = '7'
    group_drift_var = drift4
    adaptive = true
  []
  [group5_drift_adaptive]
    type = GroupDrift
    variable = group5
    block = '7'
    group_drift_var = drift5
    adaptive = true
  []
  [group6_drift_adaptive]
    type = GroupDrift
    variable = group6
    block = '7'
    group_drift_var = drift6
    adaptive = true
  []
  [group7_drift_adaptive]
    type = GroupDrift
    variable = group7
    block = '7'
    group_drift_var = drift7
    adaptive = true
  []
  [group8_drift_adaptive]
    type = GroupDrift
    variable = group8
    block = '7'
    group_drift_var = drift8
    adaptive = true
  []
[]

[Materials]
  [absorber]
    type = MoltresJsonMaterial
    base_file = '../../property_file_dir/sn-test/absorber-air-lattice-ref.json'
    material_key = 'ctrlrod'
    interp_type = 'none'
    block = '0'
  []
  [air]
    type = MoltresJsonMaterial
    base_file = '../../property_file_dir/sn-test/absorber-air-lattice-ref.json'
    material_key = 'air'
    interp_type = 'none'
    block = '1'
  []
  [fuel]
    type = MoltresJsonMaterial
    base_file = '../../property_file_dir/sn-test/absorber-air-lattice-ref.json'
    material_key = 'fuel'
    interp_type = 'none'
    block = '2 5 7'
  []
  [graphite]
    type = MoltresJsonMaterial
    base_file = '../../property_file_dir/sn-test/absorber-air-lattice-ref.json'
    material_key = 'graphite'
    interp_type = 'none'
    block = '3 6'
  []
  [inor]
    type = MoltresJsonMaterial
    base_file = '../../property_file_dir/sn-test/absorber-air-lattice-ref.json'
    material_key = 'inor'
    interp_type = 'none'
    block = '4'
  []
[]

[Executioner]
  type = Eigenvalue
  free_power_iterations = 2

  fixed_point_abs_tol = 1e-10
  fixed_point_rel_tol = 1e-8
  fixed_point_max_its = 8
  accept_on_max_fixed_point_iteration = false

  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-50

  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value = 'hypre boomeramg 200'
  line_search = 'none'
[]

[MultiApps]
  [sub]
    type = FullSolveMultiApp
    input_files = absorber-air-lattice-ref-hybrid-sub.i
    execute_on = timestep_end
    keep_solution_during_restore = true
    update_old_solution_when_keeping_solution_during_restore = false
  []
[]

[Transfers]
  [to_sub]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'group1 group2 group3 group4 group5 group6 group7 group8'
    variable = 'group1 group2 group3 group4 group5 group6 group7 group8'
    to_multi_app = sub
  []
  [to_sub_k]
    type = MultiAppPostprocessorTransfer
    from_postprocessor = eigenvalue
    to_postprocessor = eigenvalue
    to_multi_app = sub
  []
  [from_sub_drift1]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'drift1 drift1 drift1'
    variable = 'drift1 drift1 drift1'
    from_multi_app = sub
    source_variable_components = '0 1 2'
    target_variable_components = '0 1 2'
  []
  [from_sub_drift2]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'drift2 drift2 drift2'
    variable = 'drift2 drift2 drift2'
    from_multi_app = sub
    source_variable_components = '0 1 2'
    target_variable_components = '0 1 2'
  []
  [from_sub_drift3]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'drift3 drift3 drift3'
    variable = 'drift3 drift3 drift3'
    from_multi_app = sub
    source_variable_components = '0 1 2'
    target_variable_components = '0 1 2'
  []
  [from_sub_drift4]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'drift4 drift4 drift4'
    variable = 'drift4 drift4 drift4'
    from_multi_app = sub
    source_variable_components = '0 1 2'
    target_variable_components = '0 1 2'
  []
  [from_sub_drift5]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'drift5 drift5 drift5'
    variable = 'drift5 drift5 drift5'
    from_multi_app = sub
    source_variable_components = '0 1 2'
    target_variable_components = '0 1 2'
  []
  [from_sub_drift6]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'drift6 drift6 drift6'
    variable = 'drift6 drift6 drift6'
    from_multi_app = sub
    source_variable_components = '0 1 2'
    target_variable_components = '0 1 2'
  []
  [from_sub_drift7]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'drift7 drift7 drift7'
    variable = 'drift7 drift7 drift7'
    from_multi_app = sub
    source_variable_components = '0 1 2'
    target_variable_components = '0 1 2'
  []
  [from_sub_drift8]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    source_variable = 'drift8 drift8 drift8'
    variable = 'drift8 drift8 drift8'
    from_multi_app = sub
    source_variable_components = '0 1 2'
    target_variable_components = '0 1 2'
  []
[]

[Postprocessors]
  [bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    block = '2 5 7'
    execute_on = 'initial linear timestep_end'
  []
  [eigenvalue]
    type = VectorPostprocessorComponent
    vectorpostprocessor = eigenvalues
    vector_name = eigen_values_real
    index = 0
  []
[]

[VectorPostprocessors]
  [eigenvalues]
    type = Eigenvalues
    inverse_eigenvalue = true
    outputs = console
  []
[]

[Outputs]
  perf_graph = true
  [exodus]
    type = Exodus
    discontinuous = true
    execute_on = 'initial timestep_end failed'
  []
[]
