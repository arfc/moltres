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
  file = '2d_lattice_structured_smaller.msh'
[]

[Problem]
  coord_type = RZ
[]

[Variables]
  [group1]
  []
  [group2]
  []
[]

[Kernels]
  # Neutronics
  [diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  []
  [sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  []
  [fission_source_group1]
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
    extra_vector_tags = 'eigen'
  []
  [inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
  []
  [diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  []
  [sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  []
  [fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
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
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
    variable = group1
  []
  [vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
    variable = group2
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    block = 'fuel'
  []
  [moder]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_mod_'
    interp_type = 'spline'
    block = 'moder'
  []
[]

[Executioner]
  type = Eigenvalue
  initial_eigenvalue = 1
  eigen_tol = 1e-6

  solve_type = PJFNK
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type'
  petsc_options_value = 'asm lu'
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
  [multiplication_factor]
    type = VectorPostprocessorComponent
    index = 0
    vector_name = 'eigen_values_real'
    vectorpostprocessor = eigenvalues
  []
[]

[VectorPostprocessors]
  [./eigenvalues]
    type = Eigenvalues
    inverse_eigenvalue = true
    contains_complete_history = true
  []
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [out]
    type = Exodus
  []
  [csv]
    type = CSV
  []
[]

[Debug]
  show_var_residual_norms = true
[]
