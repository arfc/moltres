# Phase 1: Steady-state coupling

Like the Phase 0 results, the input files for the Phase 1 results in our
journal article [2] were run on a 200-by-200 uniform structured mesh on the
Blue Waters supercomputer with 256 MPI processes on sixteen XK or eight XE
computing nodes. We recommend having at least 32GB of
RAM to run all Phase 1 Steps on a quad-core desktop with four MPI threads,
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

Note that Steps 1.3 and 1.4 each have a neutronics sub-app input file to
loosely couple the neutronics to the thermal-hydraulics.

To change the velocity boundary conditions and power output for Step 1.4,
edit the ```BCs``` block and ```Executioner``` block in the
```full-coupling.i``` and ```full-coupling-nts.i```, respectively, according
to the instructions in the inline comments.

The k_eff results on the different mesh resolutions are:
- Step 1.1
  - 200-by-200 mesh: 1.004045
  - 40-by-40 mesh: 1.003946
- Step 1.2
  - 200-by-200 mesh: 0.992661
  - 40-by-40 mesh: 0.992553
- Step 1.3
  - 200-by-200 mesh: 0.992634
  - 40-by-40 mesh: 0.992497
- Step 1.4: U_lid = 0.5 m s-1 and P = 1 GW
  - 200-by-200 mesh: 0.992782
  - 40-by-40 mesh: 0.992655

## Step 1.1: Circulating fuel

### Input file

- ```circ-fuel.i```

### Output files

- ```circ-fuel_exodus.e```: Exodus output of the circulating fuel solution
- ```circ-fuel_csv_pre_elemental_0002.csv```: 2D, element-centered precursor
concentration values
- ```circ-fuel_csv.csv```: Contains the k_eff estimate

## Step 1.2: Power coupling

### Input file

- ```power-coupling.i```

### Output files

- ```power-coupling_exodus.e```: Exodus output of the power coupling solution
- ```power-coupling_csv_flux_aa_0003.csv```: Neutron group flux values along
the AA' centerline
- ```power-coupling_csv_flux_bb_0003.csv```: Neutron group flux values along
the BB' centerline
- ```power-coupling_csv_temp_aa_0003.csv```: Temperature values along the AA'
centerline
- ```power-coupling_csv_temp_bb_0003.csv```: Temperature values along the BB'
centerline
- ```power-coupling_csv.csv```: Contains the k_eff estimate

## Step 1.3: Buoyancy

### Input files

- ```buoyancy.i``` (Run this input file)
- ```buoyancy-nts.i``` (Neutronics sub-app input file)

### Output files

- ```buoyancy_exodus.e```: Exodus output of the temperature and velocity fields
of the buoyancy solution
- ```buoyancy_out_ntsApp0_exodus.e```: Exodus output of the neutron flux and
precursor concentrations of the buoyancy solution
- ```buoyancy_csv_temp_aa_0028.csv```: Temperature values along the AA'
centerline
- ```buoyancy_csv_temp_bb_0028.csv```: Temperature values along the BB'
centerline
- ```buoyancy_csv_vel_aa_0028.csv```: Velocity values along the AA'
centerline
- ```buoyancy_csv_vel_bb_0028.csv```: Velocity values along the BB'
centerline
- ```buoyancy_out_ntsApp0_csv_pre_elemental_0054.csv```: 2D, element-centered
precursor concentration values
- ```buoyancy_out_ntsApp0_csv.csv```: Contains the k_eff estimate

## Step 1.4: Full coupling

### Input files

- ```full-coupling.i``` (Run this input file)
- ```full-coupling-nts.i``` (Neutronics sub-app input file)

### Output files

- ```full-coupling_exodus.e```: Exodus output of the temperature and velocity fields
of the full coupling solution
- ```full-coupling_out_ntsApp0_exodus.e```: Exodus output of the neutron flux and
precursor concentrations of the full coupling solution
- ```full-coupling_out_ntsApp0_csv.csv```: Contains the k_eff estimate

[1] Tiberga M. et al., "Results from a multi-physics numerical benchmark for codes
dedicated to molten salt fast reactors", Annals of Nuclear Energy, vol. 142,
July 2020, 107428.

[2] Park S. M. & Munk M., "Verification of Moltres for Multiphysics
Simulations of Fast-Spectrum Molten Salt Reactors", Annals of Nuclear Energy, vol. 173,
August 2022, 109111.
