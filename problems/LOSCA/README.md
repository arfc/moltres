Simulates a loss of secondary cooling accident in 2D MSRE-like system.
 - uses multiapp to simulate primary loop as 1D problem
 - HX is a dirac kernel that removes heat
 - dirac HX heat removal rate is controllable, so this is set through a function to simulate LOSCA

# To simulate HX failure transient:

1. Mesh the geo file: `gmsh -2 2d_lattice_structured.geo`

2. Run the master input file `auto_diff_rho.i` in this directory:
   `mpirun -np 8 moltres-opt -i auto_diff_rho.i`

3. `cd` into the `HXFailure` directory and run the master input file
   (same name: `auto_diff_rho.i`). This input file in HXFailure will look
   in the above directory for the correct output files to kick the
   simulation off on steady-state.
