density = .002      # kg cm-3
viscosity = .5      # dynamic viscosity, mu = nu * rho, kg cm-1 s-1
alpha = 1           # INS SUPG and PSPG stabilization parameter

[GlobalParams]
  integrate_p_by_parts = true
[]

[Mesh]
  [./square]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 200
    ny = 200
    xmin = 0
    xmax = 200
    ymin = 0
    ymax = 200
    elem_type = QUAD4
  [../]
  [./corner_node]
    type = ExtraNodesetGenerator
    input = square
    new_boundary = 'pinned_node'
    coord = '200 200'
  [../]
[]

[Problem]
  type = FEProblem
[]

[Variables]
  [./vel]
    family = LAGRANGE_VEC
    order = FIRST
  [../]
  [./p]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[AuxVariables]
  [./vel_x]
    family = LAGRANGE
    order = FIRST
  [../]
  [./vel_y]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Kernels]
  [./mass]
    type = INSADMass
    variable = p
  [../]
  [./mass_pspg]
    type = INSADMassPSPG
    variable = p
  [../]
  [./momentum_advection]
    type = INSADMomentumAdvection
    variable = vel
  [../]
  [./momentum_viscous]
    type = INSADMomentumViscous
    variable = vel
  [../]
  [./momentum_pressure]
    type = INSADMomentumPressure
    variable = vel
    p = p
  [../]
  [./momentum_supg]
    type = INSADMomentumSUPG
    variable = vel
    velocity = vel
  [../]
[]

[AuxKernels]
  [./vel_x]
    type = VectorVariableComponentAux
    variable = vel_x
    vector_variable = vel
    component = 'x'
  [../]
  [./vel_y]
    type = VectorVariableComponentAux
    variable = vel_y
    vector_variable = vel
    component = 'y'
  [../]
[]

[ICs]
  [velocity]
    type = VectorConstantIC
    x_value = 1e-15
    y_value = 1e-15
    variable = vel
  []
[]

[BCs]
  [./no_slip]
    type = VectorDirichletBC
    variable = vel
    boundary = 'bottom left right'
    values = '0 0 0'
  [../]
  [./lid]
    type = VectorDirichletBC
    variable = vel
    boundary = 'top'
    values = '50 0 0'
  [../]
  [./pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  [../]
[]

[Materials]
  [./ad_mat]
    type = ADGenericConstantMaterial
    prop_names = 'rho mu'
    prop_values = '${density} ${viscosity}'
  [../]
  [./ins_mat]
    type = INSADTauMaterial
    alpha = ${alpha}
    velocity = vel
    pressure = p
  [../]
[]

[Executioner]
  type = Steady

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       NONZERO               superlu_dist'
  line_search = 'none'
[]

[VectorPostprocessors]
  [./aa]
    type = LineValueSampler
    variable = 'vel_x vel_y'
    start_point = '0 100 0'
    end_point = '200 100 0'
    num_points = 201
    sort_by = x
    execute_on = FINAL
    outputs = 'csv'
  [../]
  [./bb]
    type = LineValueSampler
    variable = 'vel_x vel_y'
    start_point = '100 0 0'
    end_point = '100 200 0'
    num_points = 201
    sort_by = y
    execute_on = FINAL
    outputs = 'csv'
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Outputs]
  [./exodus]
    type = Exodus
    execute_on = 'final'
  [../]
  [./csv]
    type = CSV
    execute_on = 'final'
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
