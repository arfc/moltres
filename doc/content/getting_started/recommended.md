# Recommended Executioner and Preconditioning Settings

This page provides recommended `Executioner` and `Preconditioning`
settings for new Moltres users who already have basic proficiency with MOOSE. The
following sections provide recommendations on the solve type, automatic scaling, time
integration scheme, and preconditioning settings.

Moltres, like all MOOSE-based apps, rely on PETSc for preconditioning. Refer to MOOSE's
documentation
[here](https://mooseframework.inl.gov/syntax/Preconditioning/index.html) for
basic information on preconditioning in MOOSE. You may also refer to the example input file
[here](http://arfc.github.io/software/moltres/wiki/input_example/) for an introduction
to the Moltres input file format.

## Solve Type

Previous experiences have shown that Moltres simulations usually require the `NEWTON`
solve type with all off-diagonal Jacobian entries enabled because reactor neutronics
problems are very non-linear from the strong coupling between the neutron group flux
and delayed neutron precursor concentration variables. Users may use these settings by
setting `solve_type = NEWTON` and `full = true` in the `Executioner` and
`Preconditioning` blocks, respectively, in the input file.

## Automatic Scaling

Relevant variables in a Moltres simulation include neutron group fluxes, delayed neutron
precursor concentrations, temperature, and velocity components. These variables differ
significantly in magnitude, especially between the neutronics and thermal-hydraulics
variables. We recommend applying scaling factors to the variables so that: 1)
final residual norm values are on similar orders of magnitudes to ensure that every
variable is converged at the end of each step, and 2) the Jacobian is well-conditioned.

Users may manually set the scaling factor for each variable using the `scaling` parameter
when defining the variable. Alternatively, we recommend using the `automatic_scaling`
feature from MOOSE. This feature automatically calculates the appropriate scaling factor
for each variable.

We recommend using the following parameters to set automatic scaling in the `Executioner` block.

```
automatic_scaling = true
resid_vs_jac_scaling_param = 0.2
scaling_group_variables = 'group1 group2; pre1 pre2 pre3 pre4 pre5 pre6 pre7 pre8; temp'
compute_scaling_once = false
off_diagonals_in_auto_scaling = true
```

- `automatic_scaling`: Turns on automatic scaling.
- `resid_vs_jac_scaling_param`: Determines whether the scaling factor is based on
  the residual or Jacobian entries; `0` corresponds to pure Jacobian scaling and `1` corresponds
  to pure residual scaling while all values in between correspond to hybrid scaling. The ideal
  value for this parameter depends heavily on the type of simulation. Most Moltres simulations run
  faster with this parameter at 0.1~0.3.
- `scaling_group_variables`: Groups variables which share the same scaling factor.
  The MOOSE team recommends grouping variables derived from the same physics for stability.
- `compute_scaling_once`: Whether Moltres calculates the scaling factors once at the beginning
  of the simulation (`true`) or at the beginning of every timestep (`false`).
- `off_diagonals_in_auto_scaling`: Whether to consider off-diagonals when determining automatic
  scaling factors.

## Time Integration Scheme

We recommend using either `ImplicitEuler` or `BDF2` based on the first and second order backward
differentiation formula time integration schemes. The `BDF2` scheme is more accurate
(higher-order) and has a
[superior convergence rate](https://mooseframework.inl.gov/workshop/#/33/4).

All multi-stage time integration schemes from MOOSE are incompatible with Moltres simulations.

To set the time integration scheme, include the following line in the `Executioner` block.

```
scheme = bdf2    # or implicit-euler for ImplicitEuler
```

## Preconditioning

Our discussions on preconditioning here relate to the `NEWTON` solve type which relies on PETSc
routines to solve the system of linear equations in each Newton iteration.

Users can pick their preferred linear system solver and the associated settings in PETSc for their
problem through the `petsc_options_iname` and `petsc_options_value` parameters in the
`Executioner` block.

#### LU

The most reliable "preconditioner" type for Moltres simulations is `lu`. `lu` is actually a direct
solver based on LU
factorization. As a direct solver, it is very accurate as long as the user provides the correct
Jacobian formulations in their kernels. PETSc's default `lu` implementation is serial and thus,
it does not scale over multiple processors. `lu` requires the `superlu_dist` parallel library to
be effective over multiple processors. However, `superlu_dist` does not scale as well as the `asm`
option in the next section. As such, we recommend only using `superlu_dist` for smaller problems
(<100k DOFs).

`lu` on a single process:

```
petsc_options_iname = '-pc_type -pc_factor_shift_type'
petsc_options_value = 'lu       NONZERO'
```

`lu` with `superlu_dist` on multiple MPI processes:

```
petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_mat_solver_type'
petsc_options_value = 'lu       NONZERO               superlu_dist'
```

#### ASM

Direct solvers like `lu` do not scale well with problem size. Iterative methods are recommended
for large problems. The best performing preconditioner type for large problems in Moltres is
`asm`. `asm` is based on the Additive Schwarz Method (ASM) for generating preconditioners for the
GMRES iterative method. Moltres simulations also require a strong subsolver like `lu` for solving
each subdomain.

`asm` on multiple MPI processes:

```
petsc_options_iname = '-pc_type -sub_pc_type -sub_pc_factor_shift_type -pc_asm_overlap -ksp_gmres_restart'
petsc_options_value = 'asm      lu           NONZERO                   1               200'
```

#### Preconditioner Recommendations

Here are the recommended preconditioner settings for the following problem sizes:

Small problems (<10k DOFs)

- Number of MPI processes: 1-4
- Preconditioner options:
  - `lu` on 1 process
  - `lu` with `superlu_dist` on multiple processes
  - `asm` on multiple processes

Medium problems (<100k DOFs)

- Number of MPI processes: 4-16
- Preconditioner options:
  - `lu` with `superlu_dist` on multiple processes
  - `asm` on multiple processes

Large problems (>100k DOFs)

- Number of MPI processes: Up to 1 MPI process per 10k DOFs
- Preconditioner options:
  - `asm` on multiple processes

## General tips

- `lu` is faster than `asm` for small problems.
- `l_tol = 1e-5` is the default linear tolerance value. `l_tol` can be raised to 1e-4 or 1e-3 for
  large problems without much impact on performance if the problem requires too many linear
  iterations (>400) on every nonlinear iteration.
- Automatic scaling may not scale correctly on the first timestep when restarting a Moltres
  simulation using MOOSE's `Restart` functionality. Thus, the simulation may fail to converge on
  the first timestep.
- When using `asm`, the number of linear iterations required per nonlinear iteration scales
  linearly with the number of subdomains (i.e. number of MPI processes). As such, increasing the
  number of processors beyond a certain threshold increases the solve time.
