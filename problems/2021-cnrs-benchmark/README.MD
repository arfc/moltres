# CNRS Benchmark

This subdirectory contains the Moltres input files to run the various subcases
in the CNRS multiphysics benchmark for MSRs [1]. The required cross section
data files are in the ```$MOLTRES/property_file_dir/cnrs-benchmark/```
subdirectory. Run the ```feedback.py``` script in that subdirectory to generate
the extrapolated cross section data based on thermal salt expansion feedback.

The domain is a homogeneous, 2m by 2m square of molten salt. AA' and
BB' refer to the horizontal and vertical centerlines across the domain. All
reported values (e.g. velocity, temperature, etc.) are measured at 0.01m
intervals along AA' or BB' unless otherwise stated.

The CNRS benchmark consists of three phases:
- Phase 0: Single physics benchmark
  - Step 0.1: Velocity field
  - Step 0.2: Neutronics
  - Step 0.3: Temperature
- Phase 1: Steady-state coupling
  - Step 1.1: Circulating fuel
  - Step 1.2: Power coupling
  - Step 1.3: Buoyancy
  - Step 1.4: Full coupling
- Phase 2: Time dependent coupling
  - Step 2.1: Forced convection transient

Refer to the ```phase-0/```, ```phase-1/```, and ```phase-2/``` for the
input files and relevant instructions on how to run them.

[1] Tiberga et al., "Results from a multi-physics numerical benchmark for codes
dedicated to molten salt fast reactors", Annals of Nuclear Energy, vol. 142,
July 2020, 107428.
