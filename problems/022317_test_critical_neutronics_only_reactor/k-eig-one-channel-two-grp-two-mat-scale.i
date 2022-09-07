global_temperature = 922

[GlobalParams]
  num_groups = 2
  num_precursor_groups = 6
  use_exp_form = false
  group_fluxes = 'group1 group2'
  temperature = ${global_temperature}
  sss2_input = false
[]

[Problem]
  coord_type = RZ
[]

[Mesh]
  file = 'cylinder_structured_for_msre_comp.msh'
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
    type = CoupledFissionEigenKernel
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
    type = CoupledFissionEigenKernel
    variable = group2
    group_number = 2
  []
  [inscatter_group1]
    type = InScatter
    variable = group1
    group_number = 1
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
    boundary = 'fuel_bottom fuel_top graphite_bottom graphite_top'
    variable = group1
  []
  [vacuum_group2]
    type = VacuumConcBC
    boundary = 'fuel_bottom fuel_top graphite_bottom graphite_top'
    variable = group2
  []
[]

[Materials]
  [fuel]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_fuel_'
    interp_type = 'spline'
    temperature = ${global_temperature}
    block = 'fuel'
  []
  [moder]
    type = GenericMoltresMaterial
    property_tables_root = '../property_file_dir/newt_msre_mod_'
    interp_type = 'spline'
    temperature = ${global_temperature}
    block = 'moder'
  []
[]

[Executioner]
  type = InversePowerMethod
  max_power_iterations = 50
  xdiff = 'group1diff'

  eig_check_tol = 1e-9
  bx_norm = 'bnorm'
  k0 = 1.5
  pfactor = 1e-2
  l_max_its = 100

  # solve_type = 'PJFNK'
  solve_type = 'NEWTON'
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
[]

[Outputs]
  perf_graph = true
  [out]
    type = Exodus
    execute_on = 'timestep_end'
  []
[]

[Debug]
  show_var_residual_norms = true
[]
