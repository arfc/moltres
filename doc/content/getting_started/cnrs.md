# Time-Dependent Simulation with Thermal-Hydraulic Coupling and Precursor Looping

The input files associated with this tutorial are found in [moltres/problems/2021-cnrs-benchmark](https://github.com/arfc/moltres/blob/devel/problems/2021-cnrs-benchmark). This benchmark is chosen for its demonstration of MSR multiphysics.

The CNRS benchmark aims to facilitate code-to-code comparison of multiphysics simulations of molten salt reactors, particularly fast-spectrum MSRs.
The benchmark is split into three phases (0, 1, and 2), with eight steps total.
Each phase investigates increasingly coupled phenomena: phase 0 models single physics (velocity, neutronics, and temperature); phase 1 models fully-coupled steady-state systems (circulating fuel, power feedback, buoyancy, and full-coupled driven-flow); and phase 2 models a transient event (linear perturbations of steady-state solution).
The benchmark geometry is shown in Figure 1.

!media media/cnrs_geo.png
       id=cnrs-geo
       caption=CNRS benchmark geometry, a 2 m by 2 m insulated cavity surrounded by vacuum. AA' and BB' are lines where observables are collected for comparison [Tiberga et al., (2020)](https://doi.org/10.1016/j.anucene.2020.107428).
       style=width:35%;text-align:center;margin-left:auto;margin-right:auto;

More detailed discussion of each phase and results of Moltres simulations of the benchmark can be found in [Verification of Moltres for Multiphysics Simulations of Fast-Spectrum Molten Salt Reactors](http://arfc.github.io/papers/park_verification_2022.pdf).

## Phase 0 Simulations

## Phase 1 Simulations

## Phase 2 Simulations