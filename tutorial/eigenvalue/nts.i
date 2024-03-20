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
  coord_type = RZ
  [mesh]
    type = FileMeshGenerator
    file = 'mesh.e'
  []
[]

[Problem]
  type = EigenProblem
  bx_norm = bnorm
[]

[Variables]
  [group1]
    order = FIRST
    family = LAGRANGE
  []
  [group2]
    order = FIRST
    family = LAGRANGE
  []
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

[Kernels]
  #---------------------------------------------------------------------
  # Group 1 Neutronics
  #---------------------------------------------------------------------
  [sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  []
  [diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  []
  [inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
  []
  [fission_source_group1]
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
    block = '0'
    extra_vector_tags = 'eigen'
  []
  [delayed_group1]
    type = DelayedNeutronSource
    variable = group1
    block = '0'
    group_number = 1
  []

  #---------------------------------------------------------------------
  # Group 2 Neutronics
  #---------------------------------------------------------------------
  [sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  []
  [diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  []
  [fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
    block = '0'
    extra_vector_tags = 'eigen'
  []
  [inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  []
[]

[BCs]
  [vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottom fuel_top mod_bottom mod_top right'
    variable = group1
  []
  [vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottom fuel_top mod_bottom mod_top right'
    variable = group2
  []
[]

[Materials]
  [fuel]
    type = MoltresJsonMaterial
    block = '0'
    base_file = 'xsdata.json'
    material_key = 'fuel'
    interp_type = LINEAR
    prop_names = ''
    prop_values = ''
  []
  [graphite]
    type = MoltresJsonMaterial
    block = '1'
    base_file = 'xsdata.json'
    material_key = 'graphite'
    interp_type = LINEAR
    prop_names = ''
    prop_values = ''
  []
[]

[Executioner]
  type = Eigenvalue
  initial_eigenvalue = 1
  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'

  automatic_scaling = true
  compute_scaling_once = false
  resid_vs_jac_scaling_param = 0.1

  line_search = none
[]

[Postprocessors]
  [k_eff]
    type = VectorPostprocessorComponent
    index = 0
    vectorpostprocessor = k_vpp
    vector_name = eigen_values_real
  []
  [bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    block = 0
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
  [k_vpp]
    type = Eigenvalues
    inverse_eigenvalue = true
  []
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
