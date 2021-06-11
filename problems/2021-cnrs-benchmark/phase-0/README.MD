# Phase 0: Single physics benchmark

The input files for the Phase 0 results in our journal article [2] were run on
a 200-by-200 uniform structured mesh on the Blue Waters supercomputer with 256
MPI processes on
sixteen XK or eight XE computing nodes. We recommend having at least 32GB of
RAM to run Steps 0.2 and 0.3 on a quad-core desktop with four MPI threads,
possibly more if running on a cluster with more computing cores.

Sample command to run the input files:
```mpirun -n 4 $MOLTRES/moltres-opt -i nts.i```

Otherwise, on a general dual/quad-core computer with 8GB of RAM, we recommend
running on a coarser 40-by-40 mesh to reduce the memory requirements with the
following preconditioner settings in the ```Executioner``` block of the input
file on a dual-core/quad-core computer with 8GB of RAM:

```
petsc_options_iname = '-pc_type -sub_pc_type -ksp_gmres_restart -pc_asm_overlap -sub_pc_factor_shift_type'
petsc_options_value = 'asm      lu           200                1                NONZERO'
```

The Step 0.2 reactivity results on the different mesh resolutions are:
- 200-by-200 mesh: 465.6 pcm
- 40-by-40 mesh: 460.7 pcm

## Step 0.1: Velocity Field

### Input file

- ```vel-field.i```

### Output files

- ```vel-field_exodus.e```: Exodus output of the velocity field solution
- ```vel-field_csv_aa_0002.csv```: Velocity values along the AA' centerline
- ```vel-field_csv_bb_0002.csv```: Velocity values along the BB' centerline

## Step 0.2: Neutronics

### Input file

- ```nts.i```

### Output files

- ```nts_exodus.e```: Exodus output of the neutron group fluxes and delayed
neutron precursor distributions
- ```nts_csv_aa_0002.csv```: Neutron group flux values along the AA'
centerline

## Step 0.3: Temperature

### Input file

- ```temperature.i```

### Output files

- ```temperature_exodus.e```: Exodus output of the temperature distribution
- ```temperature_csv_aa_0002.csv```: Temperature values along the AA'
centerline
- ```temperature_csv_bb_0002.csv```: Temperature values along the BB'
centerline

[1] Tiberga M. et al., "Results from a multi-physics numerical benchmark for codes
dedicated to molten salt fast reactors", Annals of Nuclear Energy, vol. 142,
July 2020, 107428.

[2] Park S., Munk M., & Huff K. D., "Verification of Moltres for Multiphysics
Simulations of Fast-Spectrum Molten Salt Reactors", Annals of Nuclear Energy.
(Forthcoming)
