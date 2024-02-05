# Phase 0: Single physics benchmark

The input files for the Phase 0 results in our journal article [2] were run on
a 200-by-200 uniform structured mesh on the Blue Waters supercomputer with 256
MPI processes on sixteen XK or eight XE computing nodes. All three steps can
also be easily run on general-purpose dual/quad-core computers with 16GB RAM.

Sample command to run the input files:
```mpirun -n 4 $MOLTRES/moltres-opt -i nts.i```

Otherwise, if you intend to run these input files on a less capable computer
for tutorial purposes, we recommend running on a coarser 40-by-40 mesh to
reduce the CPU and memory requirements.

The expected Step 0.2 reactivity results on the different mesh resolutions are:
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
- ```nts_csv.csv```: Contains the k_eff estimate

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

[2] Park S. M. & Munk M., "Verification of Moltres for Multiphysics
Simulations of Fast-Spectrum Molten Salt Reactors", Annals of Nuclear Energy, vol. 173,
August 2022, 109111.
