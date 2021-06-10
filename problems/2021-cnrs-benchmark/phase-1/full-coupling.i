density = .002  # kg cm-3
cp = 3075       # J kg-1 K-1, 6.15 / 2.0e-3
k = .005        # W cm-1 K-1
gamma = 1       # W cm-3 K-1, Volumetric heat transfer coefficient
viscosity = .5  # dynamic viscosity
alpha = 1       # SUPG stabilization parameter
t_alpha = 2e-4  # K-1, Thermal expansion coefficient

[GlobalParams]
  use_exp_form = false
  temperature = temp
  integrate_p_by_parts = true
[../]

[Mesh]
  [./square]
    type = GeneratedMeshGenerator
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
  [./temp]
    family = LAGRANGE
    order = FIRST
    scaling = 1e-3
  [../]
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
  [./heat]
    family = MONOMIAL
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
  [./momentum_time]
    type = INSADMomentumTimeDerivative
    variable = vel
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
  [./buoyancy]
    type = INSADBoussinesqBodyForce
    variable = vel
    # gravity vector, cm s-2
    gravity = '0 -981 0'
    alpha_name = 't_alpha'
    ref_temp = 'temp_ref'
  [../]
  [./gravity]
    type = INSADGravityForce
    variable = vel
    # gravity vector, cm s-2
    gravity = '0 -981 0'
  [../]

  [./temp_time]
    type = INSADHeatConductionTimeDerivative
    variable = temp
  [../]
  [./temp_source]
    type = INSADEnergySource
    variable = temp
    source_variable = heat
  [../]
  [./temp_advection]
    type = INSADEnergyAdvection
    variable = temp
  [../]
  [./temp_conduction]
    type = ADHeatConduction
    variable = temp
    thermal_conductivity = 'k'
  [../]
  [./temp_supg]
    type = INSADEnergySUPG
    variable = temp
    velocity = vel
  [../]
  [./temp_sink]
    type = INSADEnergyAmbientConvection
    variable = temp
    # alpha is the heat transfer coefficient.
    alpha = ${gamma}
    T_ambient = 900
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
  [./vel_ic]
    type = VectorConstantIC
    x_value = 1e-15
    y_value = 1e-15
    variable = vel
  [../]
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
    # '50 0 0' corresponds to vel_x = 50 cm s-1 along the top boundary.
    # Change to 10, 20, 30, 40 for different velocity boundary conditions
    # at U_lid = 0.1, 0.2, 0.3, 0.4 m s-1.
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
  [./fuel]
    type = GenericConstantMaterial
    prop_names = 'temp_ref'
    prop_values = '900'
  [../]
  [./ad_mat]
    type = ADGenericConstantMaterial
    prop_names = 'k rho cp mu t_alpha'
    prop_values = '${k} ${density} ${cp} ${viscosity} ${t_alpha}'
  [../]
  [./ins_temp]
    type = INSADStabilized3Eqn
    alpha = ${alpha}
    velocity = vel
    pressure = p
    temperature = temp
  [../]
[]

[Executioner]
  type = Transient
  end_time = 2000

  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_gasm_overlap -sub_pc_factor_shift_type -pc_gasm_blocks -sub_pc_factor_mat_solver_type'
  petsc_options_value = 'gasm     lu           200                1                NONZERO                   16             superlu_dist'

## Use the settings below instead if running on a desktop/small cluster
#  petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
#  petsc_options_value = 'asm      lu           200                1               NONZERO'

  nl_abs_tol = 1e-10

  dtmin = 1e-1
  dtmax = 10
  steady_state_detection = true
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-1
    cutback_factor = .5
    growth_factor = 1.5
    optimal_iterations = 10
    iteration_window = 4
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
[]

[VectorPostprocessors]
[]

[MultiApps]
  [./ntsApp]
    type = FullSolveMultiApp
    app_type = MoltresApp
    execute_on = timestep_begin
    positions = '0 0 0'
    input_files = 'full-coupling-nts.i'
  [../]
[]

[Transfers]
  [./to_sub_temp]
    type = MultiAppProjectionTransfer
    direction = to_multiapp
    multi_app = ntsApp
    source_variable = temp
    variable = temp
  [../]
  [./to_sub_vel_x]
    type = MultiAppProjectionTransfer
    direction = to_multiapp
    multi_app = ntsApp
    source_variable = vel_x
    variable = vel_x
  [../]
  [./to_sub_vel_y]
    type = MultiAppProjectionTransfer
    direction = to_multiapp
    multi_app = ntsApp
    source_variable = vel_y
    variable = vel_y
  [../]
  [./from_sub]
    type = MultiAppProjectionTransfer
    direction = from_multiapp
    multi_app = ntsApp
    source_variable = heat
    variable = heat
  [../]
[]

[Outputs]
  perf_graph = true
  print_linear_residuals = true
  [./exodus]
    type = Exodus
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
