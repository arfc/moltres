[GlobalParams]
  num_groups = 4
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4'
  temperature = 922
  sss2_input = true
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  account_delayed = false
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
[]


[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4g_fuel_rod0_'
    interp_type = 'spline'
    block = 'fuel'
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '../../tutorial/step01_groupConstants/MSREProperties/msre_gentry_4g_moder_rod0_'
    interp_type = 'spline'
    block = 'moder'
  [../]
[]

[Executioner]
  type = Eigenvalue
  eigen_tol = 1e-6
  normalization = bnorm
  normal_factor = 1
  solve_type = 'PJFNK'
  # solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  # petsc_options_iname = '-pc_type -sub_pc_type'
  # petsc_options_value = 'asm lu'
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  [./bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    execute_on = 'nonlinear linear timestep_end'
  [../]
  [./group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'nonlinear linear timestep_end'
    use_displaced_mesh = false
  [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  csv = true
  exodus = true
[]

[Debug]
  show_var_residual_norms = true
[]
