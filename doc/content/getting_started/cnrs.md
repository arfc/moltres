# Coupled Multiphysics Simulations from the CNRS Benchmark

The input files associated with this tutorial are found in
[moltres/problems/2021-cnrs-benchmark](https://github.com/arfc/moltres/blob/devel/problems/2021-cnrs-benchmark).

The CNRS benchmark aims to facilitate code-to-code comparison of multiphysics simulations of molten salt
reactors, particularly fast-spectrum MSRs. The benchmark is split into three phases (0, 1, and 2), with
eight steps total. Each phase investigates increasingly coupled phenomena: phase 0 models single physics
(velocity, neutronics, and temperature); phase 1 models fully-coupled steady-state systems (circulating
fuel, power feedback, buoyancy, and full-coupled driven-flow); and phase 2 models a transient event
(linear perturbations of steady-state solution). The benchmark geometry is shown in Figure 1.

!media media/cnrs_geo.png
       id=cnrs-geo
       caption=CNRS benchmark geometry, a 2 m by 2 m insulated cavity surrounded by vacuum. AA' and BB' are lines where observables are collected for comparison [Tiberga et al., (2020)](https://doi.org/10.1016/j.anucene.2020.107428).
       style=width:35%;text-align:center;margin-left:auto;margin-right:auto;

More detailed discussion of each phase and published results of Moltres simulations of the benchmark can be found in
[Verification of Moltres for Multiphysics Simulations of Fast-Spectrum Molten Salt Reactors](http://arfc.github.io/papers/park_verification_2022.pdf).

This tutorial is created for its demonstration of MSR multiphysics and the varying levels of coupling
possible between them. Namely, this benchmark investigates:

- One-way coupling, where one (or more) simulation output(s) serve as the input to another simulation. For example, the
  static velocity field solved for in step 0.1 serving as the input for steps 1.1 and 1.2.
- Loose coupling, which utillizes the [MultiApps](https://mooseframework.inl.gov/syntax/MultiApps/index.html) and
  [Transfers](https://mooseframework.inl.gov/syntax/Transfers/index.html) functionalities of MOOSE. This divides the
  physics in a simulation into separate equations that inform one another as in one-way coupling while a solver iterates
  both until convergence. For example, step 1.3 loosely couples velocity, temperature, and neutronics in this manner.
- Full coupling, where all physics kernels are contained within a single system to be solved for simultaneously. For 
  example, the final step 2.1 solves all neutronics and thermal hydraulics kernels in one large nonlinear system
  simultaneously.

Because there are several instances of one-way coupling and some simulations depend on the outputs of others, the phases
of this benchmark should be run in order with constant mesh sizing between parent and sub-apps as well as between steps.

## Phase 0 Simulations

Phase 0 solves each physics on its own. Each input is a building block that later steps reuse.

+Step 0.1: Velocity field+ (`vel-field.i`). A steady, isothermal, lid-driven cavity flow solved with
the [INSAD](https://mooseframework.inl.gov/modules/navier_stokes/index.html) (Incompressible Navier-Stokes Automatic
Differentiation) kernels and SUPG/PSPG stabilization. The top boundary moves at 50 cm/s, the other walls
are no-slip, and the pressure is pinned at one corner node created with `ExtraNodesetGenerator`.

+Step 0.2: Neutronics+ (`nts.i`). A $k$-eigenvalue calculation with six neutron groups and eight
precursor groups at a uniform 900 K. The fuel is stationary
(`velocity_type = constant` with zero velocity components), so the precursors decay where they are
produced. The `Nt` and `Precursors` blocks are set up as in [Tutorial 2b](getting_started/eigenvalue.md).

+Step 0.3: Temperature+ (`temperature.i`). The lid-driven flow and the energy equation are solved
together while the neutron fluxes are frozen at the Step 0.2 solution. The fluxes are loaded with a
`SolutionUserObject` and `SolutionAux` objects, and `FissionHeatSourceAux` converts them into a
volumetric heat source normalized to $10^7$ W. Heat is removed by `INSADEnergyAmbientConvection`,
a uniform sink proportional to the difference between the local temperature and 900 K, which
represents the heat exchanger of the benchmark.

## Phase 1 Simulations

Phase 1 adds the coupling paths one at a time until all fields interact at steady state:

| Step | Input | Velocity | Temperature | Neutronics |
| - | - | - | - | - |
| 1.1 | `circ-fuel.i` | Read from Step 0.1 | Uniform 900 K | Eigenvalue, precursors advected by the flow |
| 1.2 | `power-coupling.i` | Read from Step 0.1 | Solved | Eigenvalue sub-application |
| 1.3 | `buoyancy.i` | Solved, buoyancy-driven | Solved | Eigenvalue sub-application |
| 1.4 | `full-coupling.i` | Solved, lid- and buoyancy-driven | Solved | Eigenvalue sub-application |

### Step 1.1: Circulating Fuel

Step 1.1 is the Step 0.2 eigenvalue problem with the precursors carried by the Step 0.1 velocity
field. This is one-way coupling from the flow to the neutronics. The velocity components are read
into auxiliary variables, and `velocity_type = variable` tells `PrecursorAction` to advect the
precursors with them:

!listing problems/2021-cnrs-benchmark/phase-1/circ-fuel.i block=Precursors UserObjects AuxKernels

The precursors use constant monomial (`MONOMIAL`, `CONSTANT`) variables, so `PrecursorAction` adds
discontinuous Galerkin advection kernels. Setting `outlet_boundaries = ''` and
`loop_precursors = false` reflects that the cavity is closed: no precursors leave or re-enter the
domain.

### Step 1.2: Power Coupling

Step 1.2 adds two-way coupling between temperature and neutronics. The fission heat sets the
temperature, and the temperature changes the cross sections. The velocity field is still read from
Step 0.1. The parent application, `power-coupling.i`, solves only the energy equation. The
neutronics runs in a sub-application, `power-coupling-nts.i`:

!listing problems/2021-cnrs-benchmark/phase-1/power-coupling.i block=MultiApps Transfers

The two applications exchange data as follows:

- `to_sub` copies the temperature `temp` into the neutronics application. There, `temp` is an
  auxiliary variable, and `GenericMoltresMaterial` interpolates the group constants at that
  temperature.
- The neutronics application computes the fission heat source in `heat` with
  `FissionHeatSourceAux`, normalized to the total power through the `powernorm` postprocessor.
  `from_sub` copies `heat` back into the parent, where `INSADEnergySource` uses it as the source of
  the energy equation.

The parent uses a `Steady` executioner with `fixed_point_max_its = 10`. Each fixed-point (Picard)
iteration solves the energy equation, runs the full neutronics eigenvalue solve at
`execute_on = timestep_end`, and transfers the updated fields. The iterations stop when the
fixed-point convergence criteria of the executioner are met or when the iteration limit is
reached.

### Step 1.3: Buoyancy

Step 1.3 replaces the frozen velocity field with a solved one. The parent, `buoyancy.i`, solves the
momentum, mass, and energy equations together. All four walls are no-slip, so the flow is driven
only by buoyancy from the fission heating. The body force follows the Boussinesq approximation,
$\rho \vec{g} \left[1 - \alpha (T - T_{ref})\right]$, with thermal expansion coefficient
`t_alpha` and reference temperature `temp_ref` defined at 900 K. `INSADGravityForce` adds the
constant part, $\rho \vec{g}$, and `INSADBoussinesqBodyForce` adds the temperature-dependent part:

!listing problems/2021-cnrs-benchmark/phase-1/buoyancy.i block=Kernels/buoyancy Kernels/gravity

The coupling pattern is the same as in Step 1.2, plus two more transfers. The parent now also sends
the velocity components `velx` and `vely` to the sub-application, where they advect the precursors,
along with 'temp'. The parent extracts these components from its vector velocity variable `vel` with
`VectorVariableComponentAux`.

### Step 1.4: Full Coupling

Step 1.4 combines all of the above: the lid drives part of the flow, the temperature drives buoyancy, the
flow carries the precursors, and the temperature changes the cross sections.
`full-coupling.i` differs from `buoyancy.i` only in the moving-lid boundary condition:

!listing problems/2021-cnrs-benchmark/phase-1/full-coupling.i block=BCs

The parent transfers temperature and velocity to the neutronics sub-application and receives the
fission heat source:

!listing problems/2021-cnrs-benchmark/phase-1/full-coupling.i block=MultiApps Transfers

Unlike in Step 1.2, the parent uses a `Transient` executioner with `steady_state_detection = true`
and an `IterationAdaptiveDT` time stepper. This is a pseudo-transient march to the steady state.
The neutronics sub-application runs once at the end of each time step. The neutronics input,
`full-coupling-nts.i`, is a stand-alone eigenvalue problem whose temperature and velocity are
auxiliary variables filled by the transfers:

!listing problems/2021-cnrs-benchmark/phase-1/full-coupling-nts.i block=Precursors AuxKernels Executioner

## Phase 2 Simulations

Step 2.1 (`transient.i`) perturbs the Step 1.4 steady state by varying the heat exchanger
coefficient sinusoidally in time. The quantity of interest is the response of the fission power,
expressed as a gain and a phase shift relative to the perturbation. Unlike Phase 1, all physics are
in a single application: `[Variables]`, `[Nt]`, and `[Precursors]` create the velocity, pressure,
temperature, six neutron flux, and eight precursor variables in one `FEProblem`. Because there is no
eigenvalue problem in the transient, no sub-application or transfers are needed.

+Restarting from Step 1.4.+ The transient starts from the Step 1.4 solution in two parts. The
neutron fluxes and precursors come from the neutronics sub-application output
(`full-coupling_out_ntsApp0_exodus.e`). This file is loaded as the mesh with
`use_for_exodus_restart = true`, and `init_nts_from_file = true` and `init_from_file = true` in the
`Nt` and `Precursors` blocks copy the variables from it:

!listing problems/2021-cnrs-benchmark/phase-2/transient.i block=Mesh Nt Precursors

The velocity, pressure, temperature, and heat source come from the parent output
(`full-coupling_exodus.e`) through a `SolutionUserObject`, `SolutionFunction` objects, and the
`[ICs]` block.

+Making the initial state critical.+ The steady state from Step 1.4 has $k_{eff} \neq 1$, so it
would not be stationary in a transient calculation. `eigenvalue_scaling` in `[GlobalParams]`
divides the fission source by the Step 1.4 $k_{eff}$, which makes the initial state exactly
critical. The value depends on the mesh; use the commented-out value for the 40-by-40 mesh:

!listing problems/2021-cnrs-benchmark/phase-2/transient.i block=GlobalParams

+The perturbation.+ The heat sink is `ConvectiveHeatExchanger`, whose `htc` parameter is
controllable. A `RealFunctionControl` sets it at the beginning of every time step from a
`ParsedFunction`, which gives a 10% sinusoidal variation at frequency `freq`:

!listing problems/2021-cnrs-benchmark/phase-2/transient.i block=Controls Functions/func_alpha

To run other perturbation frequencies, change `freq` at the top of the input and change `dt` with
it. The time step is set to resolve each period with 200 steps.

+Fission heat.+ `FissionHeatSourceTransientAux` computes the heat source directly from the neutron
fluxes. In the transient, the flux amplitude itself represents the power level from the steady-state
solution in Step 1.4, so this heat source is not normalized.

+Solver settings.+ The coupled system is solved with `NEWTON`, BDF2 time integration, and a fully
coupled `SMP` preconditioner, so the Jacobian includes the coupling between variables. The fluxes,
precursors, velocity, pressure, and temperature have very different magnitudes. Automatic scaling
with `scaling_group_variables` computes one scaling factor for each of these groups of variables:

!listing problems/2021-cnrs-benchmark/phase-2/transient.i block=Executioner Preconditioning

The simulation takes 5000 time steps. The `checkpoint` block in `Outputs` allows a long run to be
resumed with `--recover`.

## Phase READMEs

Each phase directory contains a README that lists the running instructions, specifics of the phase, and
input/output files of each step.
