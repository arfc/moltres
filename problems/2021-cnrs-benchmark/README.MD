# CNRS Benchmark

This subdirectory contains the Moltres input files to run the various subcases
in the CNRS multiphysics benchmark for MSRs [1]. The required cross section
data files are in the ```property_file_dir/cnrs-benchmark/``` subdirectory. Run
the ```feedback.py``` script in that subdirectory to generate the extrapolated
cross section data based on thermal salt expansion feedback.

Each subsection below lists the input and output files relevant to each
subcase. The domain is a homogeneous, 2m by 2m square of molten salt. AA' and
BB' refer to the horizontal and vertical centerlines across the domain. All
reported values (e.g. velocity, temperature, etc.) are measured at 0.01m
intervals along AA' or BB' unless otherwise stated.

Step 0.1 does not require much memory to run and takes about 1 minute on a
single processor. Step 0.2 is more computationally and memory intensive; it
takes approximately 7 minutes with 256 processors on the Blue Waters
supercomputer. ```bw_job_script.pbs``` provides a template for running the
Moltres simulation on Blue Waters. Step 0.3 may be memory intensive and it
requires the neutron flux distribution from the Step 0.2 ```nts_exodus.e```
output file.

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

## Step 1.1: Circulating Fuel

TODO: Add input and output files for Step 1.1

## Step 1.2: Power Coupling

TODO: Add input and output files for Step 1.2

## Step 1.3: Buoyancy

TODO: Add input and output files for Step 1.3

## Step 1.4: Full Coupling

TODO: Add input and output files for Step 1.4

## Step 2.1: Forced Convection Transient

TODO: Add input and output files for Step 2.1

[1] Tiberga et al., "Results from a multi-physics numerical benchmark for codes
dedicated to molten salt fast reactors", Annals of Nuclear Energy, vol. 142,
July 2020, 107428.
