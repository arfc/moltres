# CNRS Benchmark

This subdirectory contains the Moltres input files to run the various subcases
in the CNRS multiphysics benchmark for MSRs (Tiberga et al., 2020).

Each subsection lists the input and output files relevant to each subcase.
The domain is a homogeneous, 2m by 2m square of molten salt. AA' and BB' refer
to the horizontal and vertical centerlines across the domain. All reported
values (e.g. velocity, temperature, etc.) are measured at 0.01m intervals
along AA' or BB' unless otherwise stated.

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
