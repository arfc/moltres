flow_velocity=21.7 # cm/s. See MSRE-properties.ods
global_temperature=922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  group_fluxes = '1 1'
  temperature = ${global_temperature}
  sss2_input = false
  transient = false
[../]

[Mesh]
  file = '2d_lattice_structured_smaller.msh'
[../]

[Problem]
  coord_type = RZ
  kernel_coverage_check = false
[]

[Precursors]
  [./pres]
    var_name_base = pre
    outlet_boundaries = 'fuel_tops'
    u_def = 0
    v_def = ${flow_velocity}
    w_def = 0
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    block = 'fuel'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
  [../]
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
  [./SMP]
    type = SMP
    full = true
  [../]
[]


[Outputs]
  print_perf_log = true
  print_linear_residuals = true
  csv = true
  [./out]
    type = Exodus
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
