flow_velocity=21.7 # cm/s. See MSRE-properties.ods

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 600
  xmax = 500
  elem_type = EDGE2
[../]

[MeshModifiers]
  [./box]
    type = SubdomainBoundingBox
    bottom_left = '250 0 0'
    top_right = '500 1 0'
    block_id = 1
  [../]
  [./interface]
    type = SideSetsBetweenSubdomains
    master_block = '0'
    paired_block = '1'
    new_boundary = 'master0_interface'
    depends_on = 'box'
  [../]
  [./interface_again]
    type = SideSetsBetweenSubdomains
    master_block = '1'
    paired_block = '0'
    new_boundary = 'master1_interface'
    depends_on = 'box'
  [../]
[]

[Variables]
  [./temp]
    initial_condition = 922 #approx steady outlet of other problem
    family = MONOMIAL
    order = CONSTANT
    block = 0
  [../]
  [./temp_right]
    initial_condition = 900 #approx steady outlet of other problem
    family = MONOMIAL
    order = CONSTANT
    block = 1
  [../]
[]

[Problem]
  kernel_coverage_check = false
[]

[Kernels]
  # # Temperature
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
  [./temp_right_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp_right
    block = 1
  [../]
[]

[DGKernels]
  [./temp_adv]
    type = DGTemperatureAdvection
    variable = temp
    velocity = '${flow_velocity} 0 0'
    block = 0
  [../]
  [./temp_right_adv]
    type = DGTemperatureAdvection
    variable = temp_right
    velocity = '${flow_velocity} 0 0'
    block = 1
  [../]
[]

[InterfaceKernels]
  [./interface]
    type = InterTemperatureAdvection
    variable = temp_right
    neighbor_var = temp
    boundary = master1_interface
    velocity = '${flow_velocity} 0 0'
    heat_source = -4e3
  [../]
[]

[BCs]
  [./core_outlet]
    boundary = 'left'
    type = PostprocessorTemperatureInflowBC
    variable = temp
    postprocessor = coreEndTemp
    uu = ${flow_velocity}
  [../]
  [./temp_advection_outlet]
    boundary = 'right'
    type = TemperatureOutflowBC
    variable = temp_right
    velocity = '${flow_velocity} 0 0'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericConstantMaterial
    prop_names = 'cp rho'
    prop_values = '1967 2.146e-3' # Robertson MSRE technical report @ 922 K
    block = '0 1'
  [../]
[]

[Postprocessors]
  [./loopEndTemp]
    type = SideAverageValue
    variable = temp_right
    boundary = 'right'
  [../]
  [./coreEndTemp]
    type = Receiver
  [../]
[]



[Executioner]
  type = Transient
  # end_time = 10000
  # dtmin = 1e-5
  # [./TimeStepper]
  #   type = IterationAdaptiveDT
  #   dt = 1e-3
  #   cutback_factor = 0.4
  #   growth_factor = 1.2
  #   optimal_iterations = 20
  # [../]
  # line_search = 'none'
  # nl_abs_tol = 1e-7

  nl_rel_tol = 1e-8

  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount'
  # petsc_options_value = 'lu	  NONZERO		1e-10'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

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
  [./exodus]
    type = Exodus
    file_base = 'sub'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
