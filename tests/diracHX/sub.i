# test that diracHX, DG kernels for heat transport are working
# test does a sinusoid heat removal in the loop
# Courant # = 0.5

flow_velocity=0.5 # cm/s. See MSRE-properties.ods

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 100
  xmax = 100
  elem_type = EDGE2
[../]

[Variables]
  [./temp]
    initial_condition = 930 #approx steady outlet of other problem
    scaling = 1e-4
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Kernels]
  # Temperature
  [./temp_time_derivative]
    type = MatINSTemperatureTimeDerivative
    variable = temp
  [../]
[]

[DGKernels]
  [./temp_adv]
    type = DGTemperatureAdvection
    variable = temp
    velocity = '${flow_velocity} 0 0'
  [../]
[]


[DiracKernels]
  [./heat_exchanger]
    type = DiracHX
    variable = temp
    power = 100 # see controls
    point = '50 0 0'
  [../]
[]


[BCs]
  [./fuel_bottoms_looped]
    boundary = 'left'
    type = TemperatureInflowBC
    variable = temp
    uu = ${flow_velocity}
    vv = 0
    ww = 0
    inlet_conc = 930
  [../]
  [./temp_advection_outlet]
    boundary = 'right'
    type = TemperatureOutflowBC
    variable = temp
    velocity = '${flow_velocity} 0 0'
  [../]
[]

[Functions]
  [./heatRemovalFcn]
    type = ParsedFunction
    expression = '1e2 + 1e2 * sin(t/5) ' # start losing cooling at t=50s
  [../]
[]

[Controls]
  [./hxFuncCtrl]
    type = RealFunctionControl
    parameter = DiracKernels/heat_exchanger/power
    function = heatRemovalFcn
    execute_on = 'initial timestep_begin'
  [../]
[]

[Materials]
  [./fuel]
    type = GenericConstantMaterial
    prop_names = 'k cp'
    prop_values = '.0553 1967' # Robertson MSRE technical report @ 922 K
  [../]
  [./rho_fuel]
    type = DerivativeParsedMaterial
    property_name = rho
    expression = '2.146e-3 * exp(-1.8 * 1.18e-4 * (temp - 922))'
    coupled_variables = 'temp'
    derivative_order = 1
  [../]
[]

[Executioner]
  type = Transient
  end_time = 100

  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-8
  scheme = crank-nicolson

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -pc_asm_overlap -sub_ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'asm      lu           1               preonly       1e-3'

  [./TimeStepper]
    type = ConstantDT
    dt = 1
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  [./temp_fuel]
    type = ElementAverageValue
    variable = temp
    outputs = 'exodus console'
  [../]
  [./loopEndTemp]
    type = SideAverageValue
    variable = temp
    boundary = 'right'
  [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [./exodus]
    type = Exodus
    file_base = 'sub'
    execute_on = 'timestep_begin'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
