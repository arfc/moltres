global_temperature=922

[GlobalParams]
  num_groups = 4
  num_precursor_groups = 8
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4'
  temperature = ${global_temperature}
[../]

[Mesh]
  type = GeneratedMesh
  dim = 2
  xmax = 80
  ymax = 80
  nx = 100
  ny = 100
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
  [./group3]
  [../]
  [./group4]
  [../]
[]

[Kernels]
  # Neutronics
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
  [./diff_group3]
    type = GroupDiffusion
    variable = group3
    group_number = 3
  [../]
  [./sigma_r_group3]
    type = SigmaR
    variable = group3
    group_number = 3
  [../]
  [./fission_source_group3]
    type = CoupledFissionEigenKernel
    variable = group3
    group_number = 3
  [../]
  [./inscatter_group3]
    type = InScatter
    variable = group3
    group_number = 3
  [../]
  [./diff_group4]
    type = GroupDiffusion
    variable = group4
    group_number = 4
  [../]
  [./sigma_r_group4]
    type = SigmaR
    variable = group4
    group_number = 4
  [../]
  [./fission_source_group4]
    type = CoupledFissionEigenKernel
    variable = group4
    group_number = 4
  [../]
  [./inscatter_group4]
    type = InScatter
    variable = group4
    group_number = 4
  [../]
[]

[Materials]
  [./homo_reactor]
    type = GenericMoltresMaterial
    block = 0
    property_tables_root = '../property_file_dir/inf_crit_small_four_groups_'
    interp_type = 'none'
    temperature = ${global_temperature}
  [../]
[]

[Executioner]
  # type = NonlinearEigen
  # free_power_iterations = 4
  # source_abs_tol = 1e-12
  # source_rel_tol = 1e-8
  # output_after_power_iterations = true


  type = InversePowerMethod
  max_power_iterations = 50
  xdiff = 'group1diff'

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
[]

[Outputs]
  [./out]
    type = Exodus
    execute_on = 'timestep_end'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
