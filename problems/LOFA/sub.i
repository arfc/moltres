flow_velocity=21.7 # cm/s. See MSRE-properties.ods
tau = 5
[Mesh]
  file = 'steady/sub.e'
[../]

[Variables]
  [./temp]
    initial_from_file_var = temp
    initial_from_file_timestep = LATEST
    family = MONOMIAL
    order = CONSTANT
    block = 0
  [../]
  [./temp_right]
    initial_from_file_var = temp_right
    initial_from_file_timestep = LATEST
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
    type = DGFunctionTemperatureAdvection
    variable = temp
    vel_x_func = coastDownFunc
    vel_y_func = nullFunc
    vel_z_func = nullFunc
    block = 0
  [../]
  [./temp_right_adv]
    type = DGFunctionTemperatureAdvection
    variable = temp_right
    vel_x_func = coastDownFunc
    vel_y_func = nullFunc
    vel_z_func = nullFunc
    block = 1
  [../]
[]

[InterfaceKernels]
  [./interface]
    type = InterTemperatureAdvection
    variable = temp_right
    neighbor_var = temp
    boundary = master1_interface
    heat_source = -4e3
    # velocity will be set through controls interface
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
    type = VelocityFunctionTemperatureOutflowBC
    variable = temp_right
    vel_x_func = coastDownFunc
    vel_y_func = nullFunc
    vel_z_func = nullFunc
  [../]
[]

[Materials]
  [./fuel]
    type = GenericConstantMaterial
    block = '0 1'
  [../]
  [./cp_fuel]
    type = DerivativeParsedMaterial
    f_name = cp
    function = '1967 + 1e-16 * temp'
    args = 'temp'
    derivative_order = 1
    block = '0 1'
  [../]
  # fuel k function
  [./k_fuel]
    type = DerivativeParsedMaterial
    f_name = k
    function = '0.0553 + 1e-16 * temp'
    args = 'temp'
    derivative_order = 1
    block = '0 1'
  [../]
  [./rho_fuel]
    type = DerivativeParsedMaterial
    f_name = rho
    function = '2.146e-3 * exp(-1.8 * 1.18e-4 * (temp - 922))'
    args = 'temp'
    derivative_order = 1
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
  dt = 1e-5
  nl_rel_tol = 1e-8
  dtmax = 10
  solve_type = 'PJFNK'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount'
  # petsc_options_value = 'lu	  NONZERO		1e-10'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

  nl_max_its = 30
  l_max_its = 100
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-3
    cutback_factor = 0.4
    growth_factor = 1.2
    optimal_iterations = 20
  [../]

[]

[Controls]
  [./flowControl]
    type = RealFunctionControl
    parameter = '*/*/u_val'
    function = coastDownFunc
    execute_on = 'timestep_begin'
  [../]
  [./flowControl2]
    type = RealFunctionControl
    parameter = '*/*/uu'
    function = coastDownFunc
    execute_on = 'timestep_begin'
  [../]
[]

[Functions]
  [./nullFunc]
    type = ParsedFunction
    value = '0'
  [../]
  [./coastDownFunc]
    type = ParsedFunction
    value = '${flow_velocity} * exp(-t / ${tau})'
  [../]
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
