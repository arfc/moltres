[GlobalParams]
  num_groups = 6
  num_precursor_groups = 8
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4 group5 group6'
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
  temperature = temp
  sss2_input = true
  account_delayed = true
[]

[Mesh]
  type = GeneratedMesh
  dim = 2

#  nx = 200
#  ny = 200
  ## Use a 40-by-40 mesh instead if running on a desktop/small cluster
  nx = 40
  ny = 40

  xmin = 0
  xmax = 200
  ymin = 0
  ymax = 200
  elem_type = QUAD4
[]

[Problem]
#  type = FEProblem
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'bottom left right top'
  create_temperature_var = false
  eigen = true
#  scaling = 1e3
[]

[Precursors]
  [pres]
    var_name_base = pre
    outlet_boundaries = ''
    constant_velocity_values = false
    uvel = velx
    vvel = vely
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    loop_precursors = false
    transient = false
    eigen = true
#    scaling = 1e3
  []
[]

[AuxVariables]
  [temp]
    family = LAGRANGE
    order = FIRST
    initial_condition = 900
  []
  [velx]
    family = LAGRANGE
    order = FIRST
  []
  [vely]
    family = LAGRANGE
    order = FIRST
  []
  [heat]
    family = MONOMIAL
    order = FIRST
  []
[]

[AuxKernels]
  [heat_source]
    type = FissionHeatSourceTransientAux
    variable = heat
#    tot_fission_heat = powernorm
#    power = 1e7
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../../property_file_dir/cnrs-benchmark/benchmark_'
    interp_type = 'linear'
  []
[]

[Executioner]
  type = Eigenvalue
  eigen_problem_type = HERMITIAN
  eigen_tol = 1e-6
  l_max_its = 100
  nl_abs_tol = 1e-12
  normalization = 'powernorm'
  normal_factor = 1e7
  automatic_scaling = true
  compute_scaling_once = false
  resid_vs_jac_scaling_param = 0.1
  scaling_group_variables = 'group1 group2 group3 group4 group5 group6; pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu superlu_dist'
#  type = InversePowerMethod
#  max_power_iterations = 50
#
#  # fission power normalization
#  # 1e7 corresponds to 1 GW / 100cm
#  # Change to 2e6, 4e6, 6e6 or 8e6 for different power output
#  # at 0.2GW, 0.4GW, 0.6GW or 0.8GW
#  normalization = 'powernorm'
#  normal_factor = 1e7 # Watts, 1e9 / 100
#
#  xdiff = 'group1diff'
#  bx_norm = 'bnorm'
#  k0 = 1.00400
#  l_max_its = 100
#  eig_check_tol = 1e-6
#
#  solve_type = 'NEWTON'
#  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
#  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_gasm_overlap -sub_pc_factor_shift_type -pc_gasm_blocks -sub_pc_factor_mat_solver_type'
#  petsc_options_value = 'gasm     lu           200                1                NONZERO                   16              superlu_dist'
#
#  ## Use the settings below instead if running on a desktop/small cluster
#  #  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
#  #  petsc_options_value = 'asm      lu           200                1               NONZERO'
#
#  line_search = none
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [max_temp]
    type = NodalExtremeValue
    value_type = max
    variable = temp
  []
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
  [pre_elemental]
    type = ElementValueSampler
    variable = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
    sort_by = id
    execute_on = TIMESTEP_END
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
