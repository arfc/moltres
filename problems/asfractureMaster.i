# Wave propogation in 1D using Newmark time integration
#
# The test is for an 1D bar element of length 4m  fixed on one end
# with a sinusoidal pulse dirichlet boundary condition applied to the other end.
# beta and gamma are Newmark time integration parameters
# The equation of motion in terms of matrices is:
#
# M*accel +  K*disp = 0
#
# Here M is the mass matrix, K is the stiffness matrix
#
# This equation is equivalent to:
#
# density*accel + Div Stress= 0
#
# The first term on the left is evaluated using the Inertial force kernel
# The last term on the left is evaluated using StressDivergenceTensors
#
# The displacement at the second, third and fourth node at t = 0.1 are
# -8.021501116638234119e-02, 2.073994362053969628e-02 and  -5.045094181261772920e-03, respectively

#[Mesh]
#  type = GeneratedMesh
#  dim = 2
#  nx = 1
#  ny = 4
#  nz = 1
#  xmin = 0.0
#  xmax = 0.1
#  ymin = 0.0
#  ymax = 4.0
#  zmin = 0.0
#  zmax = 0.1
#[]

[Mesh]
  file = outermesh.msh
#  block_id = '0'
 parallel_type = DISTRIBUTED
#  displacements = 'disp_x disp_y'
[]


[Variables]
  [./p]
  [../]
[]

[AuxVariables]
  [./vel_p]
  [../]
  [./accel_p]
  [../]
  [./accel_x_in]
  [../]
  [./accel_y_in]
  [../]
[]

[Kernels]
  # [./inertia_p]
  #   type = InertialForce
  #   variable = p
  #   velocity = vel_p
  #   acceleration = accel_p
  #   beta = 0.3025
  #   gamma = 0.6
  #   eta=0.0
  # [../]

  [./diff_p]
    type = Diffusion
    variable = p
    Diffusivity = Diff
  [../]

#  [./source_p]
#     type = SourceMonopole
#     variable = p
#     coord = '0.0 2.02 0.0'
#     size  = 0.002
#     fL = 8.33e-2
#     t1 = 0.07
#     tRT = 0.01
#     tL  = 0.8
#     p0  = 0.10
#     d1  = 9
#     upcoeff = 12.2189
#     downcoeff = 0.9404
#     rho_c = 1e-3
#  [../]
[]

# [AuxKernels]
#   [./accel_p]
#     type = NewmarkAccelAux
#     variable = accel_p
#     displacement = p
#     velocity = vel_p
#     beta = 0.3025
#     execute_on = timestep_end
#   [../]
#   [./vel_p]
#     type = NewmarkVelAux
#     variable = vel_p
#     acceleration = accel_p
#     gamma = 0.6
#     execute_on = timestep_end
#   [../]

# []


[BCs]
  [./top]
    type = NeumannBC
    variable = p
    boundary = top2
    # some_val = accel_y_in
    # Reyold  = 1.0
  [../]

  [./left]
    type = NeumannBC
    variable = p
    boundary = left2
    # some_val = accel_x_in
    # Reyold  = -1.0
  [../]

   [./right]
    type = NeumannBC
    variable = p
    boundary = right2
    # some_val = accel_x_in
    # Reyold  = 1.0
  [../]

  [./bottom]
    type = NeumannBC
    variable = p
    boundary = bottom2
    # some_val = accel_y_in
    # Reyold  = -1.0
  [../]

[]

[Materials]
  [./density]
    type = GenericConstantMaterial
    prop_names = 'density Diff'
    prop_values = '444.44 1000'
  [../]
[]

[Executioner]
  type = Transient
  start_time = 0
  end_time = 10.0
  l_tol = 1e-12
  nl_rel_tol = 1e-12
  dt = 1.667e-3
[]

[Outputs]
  csv = true
  [./console]
        type = Console
        max_rows = 10
  [../]
  [./exodus]
        type = Exodus
        interval = 5
        file_base = AcousticOutNew
  [../]
[]

#[MultiApps]
#  [./sub]
#    type = TransientMultiApp
#    app_type = ASFracture
#    execute_on = timestep_end
#    positions = '0.0 0.0 0.0'
#    input_files = asfractureDisp.i
#  [../]
#[]
#
#[Transfers]
#  [./from_sub_x]
#    type = MultiAppNearestNodeTransfer
#    direction = from_multiapp
#    multi_app = sub
#    source_variable = accel_x
#    variable = accel_x_in
#    source_boundary = 'left_right'
#    target_boundary = 'left_right'
#    execute_on = 'timestep_begin'
#  [../]

#  [./from_sub_y]
#    type = MultiAppNearestNodeTransfer
#    direction = from_multiapp
#    multi_app = sub
#    source_variable = accel_y
#    variable = accel_y_in
#    source_boundary = 'top_bottom'
#    target_boundary = 'top_bottom'
#    execute_on = 'timestep_begin'
#  [../]

#  [./to_sub_p]
#    type = MultiAppNearestNodeTransfer
#    direction = to_multiapp
#    multi_app = sub
#    source_variable = p
#    variable = pre_wave
#    source_boundary = 'inner_BC'
#    target_boundary = 'outer_BC'
#    execute_on = 'timestep_end'
#  [../]
#[]
