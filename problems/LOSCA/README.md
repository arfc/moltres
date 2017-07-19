Simulates a loss of secondary cooling accident in 2D MSRE-like system.
 - uses multiapp to simulate primary loop as 1D problem
 - HX is a dirac kernel that removes heat
 - dirac HX heat removal rate is controllable, so this is set through a function to simulate LOSCA

# To simulate HX failure transient:

1) run the input files in this directory until they come to steady-state. ctrl-c'ing out of the sim
   may be good since the temperature appears to very gently oscillate in steady-state, thus never making
   it to the 10000 s mark.
2) the input in HXFailure will look in the above directory for the correct output files to kick the simulation
   off on steady-state.
