# CNRS Benchmark

This subdirectory contains the Moltres input files to run the various subcases
in the CNRS multiphysics benchmark for MSRs (Tiberga et al., 2021).

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
