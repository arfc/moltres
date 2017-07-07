# This input file tests outflow boundary conditions for the incompressible NS equations.
width = 3.048
height = 1.016
length = 162.56
nt_scale=1e13

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  sss2_input = false
  account_delayed = false
  temperature = 922
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6'
  gamma = .0144 # Cammi .0144
  nt_scale = ${nt_scale}
[]

[Mesh]
  file = msre_squares.msh
[]


[Variables]
  [./group1]
    order = FIRST
    family = LAGRANGE
    # initial_condition = 1
    # scaling = 1e4
  [../]
  [./group2]
    order = FIRST
    family = LAGRANGE
    # initial_condition = 1
    # scaling = 1e4
  [../]
[]

# [PrecursorKernel]
#   var_name_base = pre
#   block = 'fuel'
#   outlet_boundaries = 'fuel_top'
#   u_func = vel_x_func
#   v_func = vel_y_func
#   w_func = vel_z_func
#   constant_velocity_values = false
#   nt_exp_form = false
#   family = MONOMIAL
#   order = CONSTANT
#   # jac_test = true
# []

[Kernels]
  # Neutronics
  # [./time_group1]
  #   type = NtTimeDerivative
  #   variable = group1
  #   group_number = 1
  # [../]
  [./diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
  [../]
  [./sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
  [../]
  [./fission_source_group1]
    type = CoupledFissionEigenKernel
    variable = group1
    group_number = 1
  [../]
  # [./delayed_group1]
  #   type = DelayedNeutronSource
  #   variable = group1
  # [../]
  [./inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
  [../]
  [./diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
  [../]
  [./sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
  [../]
  [./fission_source_group2]
    type = CoupledFissionEigenKernel
    variable = group2
    group_number = 2
  [../]
  [./inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
  [../]
  # [./time_group2]
  #   type = NtTimeDerivative
  #   variable = group2
  #   group_number = 2
  # [../]
[]

[BCs]
  [./vacuum_group1]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moderator_bottoms moderator_tops'
    # boundary = 'fuel_bottom moderator_bottoms'
    variable = group1
  [../]
  [./vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottoms fuel_tops moderator_bottoms moderator_tops'
    # boundary = 'fuel_bottom moderator_bottoms'
    variable = group2
  [../]
[]

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    block = 'fuel'
    prop_names = 'k cp rho'
    prop_values = '.0553 1967 2.146e-3' # Robertson MSRE technical report @ 922 K
  [../]
  [./moder]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_mod_'
    interp_type = 'spline'
    prop_names = 'k cp rho'
    prop_values = '.312 1760 1.86e-3' # Cammi 2011 at 908 K
    block = 'moderator'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

[Preconditioning]
  [./SMP_PJFNK]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = InversePowerMethod
  max_power_iterations = 50
  xdiff = 'group1diff'

  bx_norm = 'bnorm'
  k0 = 2.0
  pfactor = 1e-2
  l_max_its = 100

  # solve_type = 'PJFNK'
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
[]

[Outputs]
  print_perf_log = true
  exodus = true
  csv = true
  file_base = 'out'
[]

[Functions]
  [./nt_ic]
    type = ParsedFunction
    value = '10 * sin(pi * z / ${length})'
  [../]
[]

[ICs]
  [./group1]
    type = FunctionIC
    variable = group1
    function = nt_ic
  [../]
  [./group2]
    type = FunctionIC
    variable = group2
    function = nt_ic
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
