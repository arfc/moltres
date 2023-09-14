# Phase 2: Time dependent coupling

The input files for the Phase 2 results in our
journal article [2] were run on a 200-by-200 uniform structured mesh on the
Blue Waters supercomputer with 512 MPI processes on 32 XK or 16
XE computing nodes. We recommend running these time dependent simulations on
clusters with at least 64 independent processors and 64 GB of RAM. The
transient case with the shortest perturbation period
took ~0.67h to complete one period (200 timesteps) and 16.67h in total (5000
timesteps) to reach reasonable convergence of the gain and phase shift
values on the 512 MPI processes on Blue Water. Conversely, the case with the
longest perturbation period took x2.25 longer due to the longer simulation time
needed to reach convergence.

Sample command to run the input files:
```mpirun -n 64 $MOLTRES/moltres-opt -i transient.i```

Otherwise, on a general dual/quad-core computer with 8GB of RAM, we recommend
running on a coarser 40-by-40 mesh to reduce the memory requirements with the
following preconditioner settings in the ```Executioner``` block of the input
file on a dual-core/quad-core computer with 8GB of RAM:

```
petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
petsc_options_value = 'asm      lu           200                1               NONZERO'
```

Refer to the GitHub repository `arfc/2021-park-moltres-benchmark` for the
python scripts to calculate the gain and phase shift from the output data.

## Step 2.1: Forced convection transient

### Input files

- ```transient.i``` for perturbation frequency f = 0.8 (Change the "freq" and "dt" parameters at
the top of the input file to run different perturbation frequencies.

### Output files

- ```transient_exodus.e```: Exodus output of the forced convection
transient solution
- ```transient_csv.csv```: CSV output of the fission power output at
every timestep

[1] Tiberga M. et al., "Results from a multi-physics numerical benchmark for codes
dedicated to molten salt fast reactors", Annals of Nuclear Energy, vol. 142,
July 2020, 107428.

[2] Park S. M., Munk M., & Huff K. D., "Verification of Moltres for Multiphysics
Simulations of Fast-Spectrum Molten Salt Reactors", Annals of Nuclear Energy, vol. 173,
August 2022, 109111.
