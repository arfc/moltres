[Mesh]
  file = 'msr.msh'
[../]

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

  [./fission_source_u_from_v]
    type = CoupledFissionEigenKernel
    variable = u
    fissioning_group = v
    nu_f = nu_f_v
    sigma_f = sigma_f_v
  [../]
  [./fission_source_u_from_u]
    type = SelfFissionEigenKernel
    variable = u
    nu_f = nu_f_u
    sigma_f = sigma_f_u
  [../]
[]

[Materials]
  [./fuel]
    type = GenericConstantMaterial
    block = 'fuel'
    prop_names   =  'd_u       d_v       sigma_r_u  sigma_r_v  sigma_12   nu_f_u   sigma_f_u  nu_f_v   sigma_f_v'
    prop_values  =  '.5941650  .9912190  3.2802e-3  1.8537e-2  1.6001e-3  2.43575  8.7632e-4  2.43620  1.3699e-2'
  [../]
  [./moder]
    type = GenericConstantMaterial
    block = 'moder'
    prop_names   =  'd_u       d_v       sigma_r_u  sigma_r_v  sigma_12   nu_f_u  sigma_f_u  nu_f_v  sigma_f_v'
    prop_values  =  '.8277580  .0307684  3.0658e-3  2.3705e-3  2.2285e-3  0       0          0       0'
  [../]
[]

[BCs]
  [./homogeneous_u]
    type = DirichletBC
    variable = u
    boundary = 'boundary'
    value = 0
  [../]
  [./homogeneous_v]
    type = DirichletBC
    variable = v
    boundary = 'boundary'
    value = 0
  [../]
[]

[Executioner]
  type = NonlinearEigen

  bx_norm = 'vnorm'

  free_power_iterations = 2
  source_abs_tol = 1e-12
  source_rel_tol = 1e-50
  k0 = 1.0
  output_after_power_iterations = true

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
  [./out]
    type = Exodus
    execute_on = 'timestep_end'
  [../]
[]
