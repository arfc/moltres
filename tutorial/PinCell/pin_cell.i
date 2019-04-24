global_temperature=600

[GlobalParams]
  num_groups =2 
  base_file = 'PinXS.h5'
  group_fluxes = ' group1  group2' 
  num_precursor_groups = 6
  use_exp_form = false
  temperature = ${global_temperature}
[../]

[Mesh]
  file = pin_cell.e 
[../]

[Problem]
  type = FEProblem
  coord_type = XYZ
[]

[Nt]
  var_name_base = group
  create_temperature_var = false
  eigen = true
  account_delayed = 0
  sss2_input = 1
[]

[Materials]
  [./F]
    type = H5MoltresMaterial
    block = 1
    material_key = 'F'
    interp_type = 'NONE'
    temperature = 600
  [../]
  [./C]
    type = H5MoltresMaterial
    block = 3
    material_key = 'C'
    interp_type = 'NONE'
    temperature = 600
  [../]
  [./W]
    type = H5MoltresMaterial
    block = 4
    material_key = 'W'
    interp_type = 'NONE'
    temperature = 600
  [../]
[]

[Executioner]
   #type = NonlinearEigen
   #free_power_iterations = 4
   #source_abs_tol = 1e-12
   #source_rel_tol = 1e-8
   #output_after_power_iterations = true


  type = NonlinearEigen
  max_power_iterations = 50
  xdiff = 'group1diff'

  bx_norm = 'bnorm'
  k0 = 1.0
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

[Outputs]
  [./out]
    type = Exodus
    execute_on = 'timestep_end'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
