[Mesh]
 type = GeneratedMesh
 dim = 1
 xmin = 0
 xmax = 1.25
 nx = 16
[]

[MeshModifiers]
  [./uranium]
    type = SubdomainBoundingBox
    bottom_left = '.3125 0 0 '
    top_right = '.9375 1 0'
    block_id = 1
  [../]
  [./right_water]
    type = SubdomainBoundingBox
    bottom_left = '.9375 0 0'
    top_right = '1.25 1 0'
    block_id = 2
  [../]
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
  [../]
  [./v]
    order = FIRST
    family = LAGRANGE
    eigen = true
  [../]
[]

[Kernels]
  [./diff_u]
    type = MatDiffusion
    variable = u
    prop_name = d_u
  [../]
  [./diff_v]
    type = MatDiffusion
    variable = v
    prop_name = d_v
  [../]
  [./sigma_r_u]
    type = SigmaR
    variable = u
    sigma_r = sigma_r_u
  [../]
  [./sigma_r_v]
    type = SigmaR
    variable = v
    sigma_r = sigma_r_v
  [../]
  [./inscatter_v]
    type = InScatter
    variable = v
    scattered_group = u
    sigma_s = sigma_12
  [../]

  [./fission_source_u]
    type = CoupledFissionEigenKernel
    variable = u
    fissioning_group = v
    nu_f = nu_f_v
    sigma_f = sigma_f_v
  [../]
[]

[Materials]
  [./water]
    type = GenericConstantMaterial
    block = '0 2'
    prop_names   =  'd_u   d_v     sigma_r_u  sigma_r_v  sigma_12  nu_f_v  sigma_f_v'
    prop_values  =  '1.67  .30303  .03        0          .03       0       0'
  [../]
  [./fuel]
    type = GenericConstantMaterial
    block = 1
    prop_names   =  'd_u   d_v    sigma_r_u  sigma_r_v  sigma_12  nu_f_v  sigma_f_v'
    prop_values  =  '1.67  .3333  .015       .1         .015      1.5     .1'
  [../]
[]

# [BCs]
#   [./homogeneous_u]
#     type = DirichletBC
#     variable = u
#     boundary = '0 1 2 3'
#     value = 0
#   [../]
#   [./homogeneous_v]
#     type = DirichletBC
#     variable = v
#     boundary = '0 1 2 3'
#     value = 0
#   [../]
# []

[Executioner]
  type = NonlinearEigen

  bx_norm = 'vnorm'

  free_power_iterations = 2
  source_abs_tol = 1e-12
  source_rel_tol = 1e-50
  k0 = 1.0
  output_after_power_iterations = false

  #Preconditioned JFNK (default)
  solve_type = 'PJFNK'
[]

[Postprocessors]
  [./vnorm]
    type = ElementIntegralVariablePostprocessor
    variable = v
    # execute on residual is important for nonlinear eigen solver!
    execute_on = linear
  [../]

  [./udiff]
    type = ElementL2Diff
    variable = u
    outputs = console
  [../]
[]

[Outputs]
  execute_on = 'timestep_end'
  file_base = ne_deficient_b
  exodus = true
[]
