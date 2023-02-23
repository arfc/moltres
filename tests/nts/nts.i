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
  coord_type = RZ
  file = '2d_lattice_structured_smaller.msh'
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'fuel_bottoms fuel_tops moder_bottoms moder_tops outer_wall'
  create_temperature_var = false
  eigen = true
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
  eigen_tol = 1e-6
  normalization = fiss_neutrons
  normal_factor = 1
  solve_type = 'PJFNK'
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
  [tot_fiss]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  []
  [group1norm]
    type = ElementIntegralVariablePostprocessor
    variable = group1
  []
  [group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
  []
[]

[VectorPostprocessors]
  [k_vpp]
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
[]

[Debug]
  show_var_residual_norms = true
[]
