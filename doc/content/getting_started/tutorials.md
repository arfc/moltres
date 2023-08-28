# Tutorials

The *two-step procedure* is a common approach for multiphysics full-core nuclear reactor analysis.
These steps are:

+Step 1:+ Generate neutron group constant data with a lattice or full-core reactor model on a
high-fidelity, continuous/fine-energy neutronics software at various reactor states.

+Step 2:+ Use an intermediate-fidelity, computationally cheaper neutronics software with the
neutron group constant data to perform multiphysics reactor analysis.

Moltres falls under Step 2 of the two-step procedure. The following sections cover
various tutorials for group constant file generation and running various types of reactor
simulations with Moltres. The tutorials assume that readers have a basic understanding of reactor
analysis, molten salt reactors, and the [MOOSE framework](https://mooseframework.inl.gov/).

## 1. Group Constant File Generation

Moltres requires group constants in compatible text- or JSON-based format to run reactor
simulations. The Moltres GitHub repository provides Python scripts that can automatically parse
output files from OpenMC, Serpent 2 and NEWT (in SCALE) and generate the requisite .txt or .json
files. The newer [`moltres_xs.py`](https://github.com/arfc/moltres/blob/devel/python/moltres_xs.py)
script supports group constant file generation with all three listed neutron transport software,
while the older
[`extractSerpent2GCs.py`](https://github.com/arfc/moltres/blob/devel/python/extractSerpent2GCs.py)
script supports Serpent 2 only.

+1a.+ [Group Constant File Generation with moltres_xs.py (Recommended)](moltres_xs.md)

+1b.+ [Group Constant File Generation with extractSerpent2GCs.py](sss2_gc.md)

## 2. Multiphysics Reactor Simulations

Moltres supports various capabilities for reactor simulations including eigenvalue calculation,
neutronics/thermal-hydraulics coupling, precursor drift, and out-of-core precursor decay. Moltres
can couple different physics/components through full coupling or iterative tight
coupling.

This tutorial webpage is currently a work in progress. Feel free to post on our GitHub
[Discussions](https://github.com/arfc/moltres/discussions) page if your use case is not covered by
the existing tutorials below.

+2a.+ [Introduction to the Moltres Input File Syntax](getting_started/input_syntax.md)

+2b.+ [Eigenvalue Calculation](getting_started/eigenvalue.md)

+2c.+ [Time-dependent Simulation with Thermal-Hydraulic Coupling and Precursor Looping](getting_started/transient.md)

+Tip:+ [Recommended Executioner and Preconditioning Settings](getting_started/recommended.md)

## 3. Data Postprocessing and Analysis

Tutorials to come at a later date.
