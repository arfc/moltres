# Step 1.1: Circulating fuel input file
# Requires velocity fields from Step 0.1

[GlobalParams]
  num_groups = 6
  num_precursor_groups = 8
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4 group5 group6'
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
  temperature = 900
  sss2_input = true
  account_delayed = true
[../]

[Mesh]
    type = GeneratedMesh
    dim = 2

    nx = 200
    ny = 200
## Use a 40-by-40 mesh instead if running on a desktop/small cluster
#    nx = 40
#    ny = 40

    xmin = 0
    xmax = 200
    ymin = 0
    ymax = 200
    elem_type = QUAD4
[]

[Problem]
  type = FEProblem
[]

[Nt]
  var_name_base = group
  vacuum_boundaries = 'bottom left right top'
  create_temperature_var = false
  transient = false
  eigen = true
[]

[Precursors]
  [./pres]
    var_name_base = pre
    outlet_boundaries = ''
    constant_velocity_values = false
    uvel = ux
    vvel = uy
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    loop_precursors = false
    transient = false
  [../]
[]

[AuxVariables]
  [./ux]
    family = LAGRANGE
    order = FIRST
  [../]
  [./uy]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[UserObjects]
  [./velocities]
    type = SolutionUserObject
    mesh = '../phase-0/vel-field_exodus.e'
    system_variables = 'vel_x vel_y'
    timestep = LATEST
    execute_on = INITIAL
  [../]
[]

[AuxKernels]
  [./ux]
    type = SolutionAux
    variable = ux
    from_variable = vel_x
    solution = velocities
  [../]
  [./uy]
    type = SolutionAux
    variable = uy
    from_variable = vel_y
    solution = velocities
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../../../property_file_dir/cnrs-benchmark/benchmark_'
    interp_type = 'linear'
  [../]
[]

[Executioner]
  type = InversePowerMethod
  max_power_iterations = 50

  # fission power normalization
  normalization = 'powernorm'
  normal_factor = 1e7           # Watts, 1e9 / 100
  # Tiberga et al. assumes the depth of their 2D domain is 1m.
  # Tiberga et al. domain = 2m x 2m x 1m
  # We divide the total power=1e9W by 100 because our length units are in cm.
  # Our domain = 2m x 2m x 0.01m

  xdiff = 'group1diff'
  bx_norm = 'bnorm'
  k0 = 1.00400
  l_max_its = 100
  eig_check_tol = 1e-7

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_gasm_overlap -sub_pc_factor_shift_type -pc_gasm_blocks -sub_pc_factor_mat_solver_type'
  petsc_options_value = 'gasm     lu           200                1                NONZERO                   16              superlu_dist'

## Use the settings below instead if running on a desktop/small cluster
#  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
#  petsc_options_value = 'asm      lu           200                1               NONZERO'

  line_search = none
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
    execute_on = linear
  [../]
  [./tot_fissions]
    type = ElmIntegTotFissPostprocessor
    execute_on = linear
  [../]
  [./powernorm]
    type = ElmIntegTotFissHeatPostprocessor
    execute_on = linear
  [../]
  [./group1norm]
    type = ElementIntegralVariablePostprocessor
    variable = group1
    execute_on = linear
  [../]
  [./group1max]
    type = NodalMaxValue
    variable = group1
    execute_on = timestep_end
  [../]
  [./group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
  [./group2norm]
    type = ElementIntegralVariablePostprocessor
    variable = group2
    execute_on = linear
  [../]
  [./group2max]
    type = NodalMaxValue
    variable = group2
    execute_on = timestep_end
  [../]
  [./group2diff]
    type = ElementL2Diff
    variable = group2
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
[]

[VectorPostprocessors]
  [./pre_elemental]
    type = ElementValueSampler
    variable = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
    sort_by = id
    execute_on = FINAL
  [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [./exodus]
    type = Exodus
  [../]
  [./csv]
    type = CSV
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
