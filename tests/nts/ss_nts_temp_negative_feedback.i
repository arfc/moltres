flow_velocity=147 # Cammi 147 cm/s
inlet_temp=824
nt_scale=1e13

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 8
  group_fluxes = 'group1 group2'
[../]

[Mesh]
  file = 'axisymm_cylinder.msh'
[../]

[Variables]
  [./group1]
    order = FIRST
    family = LAGRANGE
  [../]
  [./group2]
    order = FIRST
    family = LAGRANGE
  [../]
  [./temp]
    order = FIRST
    family = LAGRANGE
    # scaling = 1e-3
  [../]
[]

[Kernels]
  # Neutronics
  [./time_group1]
    type = NtTimeDerivative
    group_number = 1
    variable = group1
    use_exp_form = false
  [../]
  [./time_group2]
    type = NtTimeDerivative
    group_number = 2
    variable = group2
    use_exp_form = false
  [../]
  [./diff_group1]
    type = GroupDiffusion
    variable = group1
    group_number = 1
    temperature = temp
    use_exp_form = false
  [../]
  [./diff_group2]
    type = GroupDiffusion
    variable = group2
    group_number = 2
    temperature = temp
    use_exp_form = false
  [../]
  [./sigma_r_group1]
    type = SigmaR
    variable = group1
    group_number = 1
    temperature = temp
    use_exp_form = false
  [../]
  [./sigma_r_group2]
    type = SigmaR
    variable = group2
    group_number = 2
    temperature = temp
    use_exp_form = false
  [../]
  [./inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
    num_groups = 2
    group_fluxes = 'group1 group2'
    temperature = temp
    use_exp_form = false
  [../]
  [./inscatter_group2]
    type = InScatter
    variable = group2
    group_number = 2
    num_groups = 2
    group_fluxes = 'group1 group2'
    temperature = temp
    use_exp_form = false
  [../]
  [./fission_source_group1]
    type = CoupledFissionKernel
    variable = group1
    group_number = 1
    num_groups = 2
    group_fluxes = 'group1 group2'
    temperature = temp
    use_exp_form = false
  [../]
  [./fission_source_group2]
    type = CoupledFissionKernel
    variable = group2
    group_number = 2
    num_groups = 2
    group_fluxes = 'group1 group2'
    temperature = temp
    use_exp_form = false
  [../]

  # Temperature
  [./temp_flow_fuel]
    block = 'fuel'
    type = MatINSTemperatureRZ
    variable = temp
    rho = 'rho'
    k = 'k'
    cp = 'cp'
    uz = ${flow_velocity}
  [../]
  [./temp_flow_moder]
    block = 'moder'
    type = MatINSTemperatureRZ
    variable = temp
    rho = 'rho'
    k = 'k'
    cp = 'cp'
  [../]
  [./temp_source]
    type = TransientFissionHeatSource
    variable = temp
    nt_scale = ${nt_scale}
  [../]
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
    rho = 'rho'
    cp = 'cp'
  [../]
[]

# Delayed neutron precursors

# [PrecursorKernel]
#   var_name_base = pre
#   v_def = ${flow_velocity}
#   block = 'fuel'
#   inlet_boundary = 'fuel_bottom'
#   inlet_boundary_condition = 'DirichletBC'
#   inlet_bc_value = -20
#   outlet_boundary = 'fuel_top'
#   temperature = temp
#   incompressible_flow = false
#   transient_simulation = true
#   use_exp_form = true
#   initial_condition = -20
# []

# [AuxVariables]
# []

# [AuxKernels]
# []

[Materials]
  [./fuel]
    type = GenericMoltresMaterial
    block = 'fuel'
    property_tables_root = 'msr2g_enrU_mod_953_fuel_interp_'
    num_groups = 2
    num_precursor_groups = 8
    prop_names = 'k rho cp'
    prop_values = '.0123 3.327e-3 1357' # Cammi 2011 at 908 K
    temperature = temp
  [../]
  [./moder]
    type = GenericMoltresMaterial
    block = 'moder'
    property_tables_root = 'msr2g_enrU_fuel_922_mod_interp_'
    num_groups = 2
    num_precursor_groups = 8
    prop_names = 'k rho cp'
    prop_values = '.312 1.843e-3 1760' # Cammi 2011 at 908 K
    temperature = temp
  [../]
[]

[BCs]
  [./temp_inlet]
    boundary = 'fuel_bottom graphite_bottom'
    type = DirichletBC
    variable = temp
    value = ${inlet_temp}
  [../]
  [./temp_outlet]
    boundary = 'fuel_top'
    type = MatINSTemperatureNoBCBC
    variable = temp
    k = 'k'
  [../]
  [./group1_vacuum]
    type = VacuumBC
    variable = group1
    boundary = 'fuel_top graphite_top fuel_bottom graphite_bottom'
  [../]
  [./group2_vacuum]
    type = VacuumBC
    variable = group2
    boundary = 'fuel_top graphite_top fuel_bottom graphite_bottom'
  [../]
[]

[Problem]
  type = FEProblem
  coord_type = RZ
[../]

[Executioner]
  type = Transient
  end_time = 10000

  nl_abs_tol = 4e-9
  trans_ss_check = true
  ss_check_tol = 4e-9

  # solve_type = 'PJFNK'
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # This system will not converge with default preconditioning; need to use asm
  # petsc_options_iname = '-pc_type -sub_pc_type -sub_ksp_type -pc_asm_overlap -ksp_gmres_restart -snes_linesearch_mindlambda'
  # petsc_options_value = 'asm lu preonly 2 31 1e-3'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda -pc_factor_mat_solver_package'
  petsc_options_value = 'lu NONZERO 1.e-10 preonly 1e-3 superlu_dist'

  nl_max_its = 10
  l_max_its = 10

  dtmin = 1e-7
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-4
    growth_factor = 1.2
    optimal_iterations = 20
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Outputs]
  csv = true
  [./out]
    type = Exodus
    execute_on = 'initial timestep_end'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]

[ICs]
  [./temp_ic]
    type = ConstantIC
    variable = temp
    value = ${inlet_temp}
  [../]
  [./group1_ic]
    type = ConstantIC
    variable = group1
    value = 1
  [../]
  [./group2_ic]
    type = ConstantIC
    variable = group2
    value = 1
  [../]
[]

[Postprocessors]
  [./group1_current]
    type = ElementIntegralVariablePostprocessor
    variable = group1
    outputs = 'csv console'
    # outputs = 'csv'
  [../]
  [./group1_old]
    type = IntegralOldVariablePostprocessor
    variable = group1
    outputs = 'csv console'
    # outputs = 'csv'
  [../]
  [./multiplication]
    type = DivisionPostprocessor
    value1 = group1_current
    value2 = group1_old
    outputs = 'csv console'
    # outputs = 'csv'
  [../]
[]