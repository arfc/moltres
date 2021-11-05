# Phase 2: Time dependent coupling

The input files for the Phase 2 results in our
journal article [2] were run on a 200-by-200 uniform structured mesh on the
Blue Waters supercomputer with 512 MPI processes on 32 XK or 16
XE computing nodes. We recommend running these time dependent simulations on
clusters with at least 64 independent processors and 64 GB of RAM. The
transient case with the shortest perturbation period
(`transient-perturb-7.i`) took ~24h to complete
5000 timesteps and reach reasonable convergence of the gain and phase shift
values on the 512 MPI processes on Blue Water. `transient-perturb-1.i` took
~125h to complete 4000 timesteps under the same conditions.

Sample command to run the input files:
```mpirun -n 64 $MOLTRES/moltres-opt -i transient-perturb-7.i```

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

- ```transient-perturb-#.i``` where `#` ranges from 1 to 7 corresponding to
perturbation frequencies f=[0.0125, 0.025, 0.05, 0.1, 0.2, 0.4, 0.8]

### Output files

- ```transient-perturb-#_exodus.e```: Exodus output of the forced convection
transient solution
- ```transient-perturb-#_csv.csv```: CSV output of the fission power output at
every timestep

[1] Tiberga M. et al., "Results from a multi-physics numerical benchmark for codes
dedicated to molten salt fast reactors", Annals of Nuclear Energy, vol. 142,
July 2020, 107428.

[2] Park S., Munk M., & Huff K. D., "Verification of Moltres for Multiphysics
Simulations of Fast-Spectrum Molten Salt Reactors". (Manuscript in progress)
