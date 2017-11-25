library_path=/home/gavin/projects/moose/modules/navier_stokes/lib/

[GlobalParams]
  # neutronics
  num_groups=6
  num_precursor_groups=8
  use_exp_form = false
  group_fluxes = 'group1 group2 group3 group4 group5 group6'
  temperature = temp
  sss2_input = false
  pre_concs = 'pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8'
  account_delayed = true
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  xmax = 1.0
  ymin = 0
  ymax = 1.0
  elem_type = QUAD9
  nx = 20
  ny = 20
[]

[Variables]
  # temperature
  [./temp]
    family = LAGRANGE
    order = FIRST
    initial_condition = 900
    scaling = 1e-4
  [../]
[]

# nt -> neutron
[Nt]
  var_name_base = group
  vacuum_boundaries = 'top bottom left right'
  create_temperature_var = false
  eigen = true
  scaling = 1e-10
[]

[Precursors]
  [./preblock]
    var_name_base = pre
    # block = 'fuel'
    constant_velocity_values = false
    nt_exp_form = false
    family = MONOMIAL
    order = CONSTANT
    outlet_boundaries = ''
    transient = false
    u_func = 0.0
    v_func = 0.0
    w_func = 0.0
  [../]
[]

[Kernels]
    [./heating]
        type = TransientFissionHeatSource
        variable = temp
        group_fluxes = 'group1 group2 group3 group4 group5 group6'
    [../]

    [./heatRemoval]
      type = ManuHX
      variable = temp
      tref = 900.0
      htc  = 1.0
    [../]
[]

[BCs]
[]

[Materials]
  # alpha = coefficient of thermal expansion where rho  = rho0 -alpha * rho0 * delta T
  # Dr. Aufiero uses nu rather than mu, (kinematic rather than absolute viscosity)
  # likewise, \rho c_p is given rather than separate \rho, c_p
  # accordingly, we let \rho = 1 to obtain equivalent results
  # also, Manu gives a length expansion coefficient. Boussinesq approximations
  # typically use an expansion coefficient with units 1/K, but for mass instead.
  # The relation between the two is alpha = -3 beta, if linearized.
  # Also, had to use Pr = mu * cp / k
  [./const]
    type = GenericConstantMaterial
    prop_names = 'mu     rho alpha   k         cp'
    prop_values = '0.025 1.0 6.0e-4  0.5  6.15e6'
  [../]

  [./fuelneutronicproperties]
    type = GenericMoltresMaterial
    property_tables_root = './groupconstants/'
    interp_type = 'none' # 'spline'
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    solve_type = 'PJFNK'
  [../]
[]

[Executioner]
  type = InversePowerMethod
  max_power_iterations = 50

  # fission power normalization:
  normalization = powernorm
  normal_factor = 1e9 # watts

  # power iteration options
  xdiff = 'group1diff'
  bx_norm = 'bnorm'
  pfactor = 1e-2

  petsc_options = '-snes_converged_reason -ksp_converged_reason'
  petsc_options_iname = '-pc_type -pc_factor_shift_type'
  petsc_options_value = 'lu NONZERO'
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-13
  nl_max_its = 6
  l_tol = 1e-6
  l_max_its = 100
[]

[Outputs]
  [./exodus]
    type = Exodus
  [../]
  [./csv]
    type = CSV
  [../]
[]

[Postprocessors]
  [./bnorm]
    type = ElmIntegTotFissNtsPostprocessor
    execute_on = linear
  [../]
  [./group1diff]
    type = ElementL2Diff
    variable = group1
    execute_on = 'linear timestep_end'
    use_displaced_mesh = false
  [../]
  [./powernorm]
    type = ElmIntegTotFissHeatPostprocessor
    execute_on = linear
  [../]
[]

[AuxVariables]
  [./deltaT]
    type = ConstantDifferenceAux
    variable = deltaT
    comparevar = temp
    constant = 900
  [../]
[]

[AuxKernels]
  [./deltaTCalc]
    type =  ConstantDifferenceAux
    variable = deltaT
    compareVar = temp
    constant = 900
  [../]
[]

[Debug]
  show_var_residual_norms = true
[]
