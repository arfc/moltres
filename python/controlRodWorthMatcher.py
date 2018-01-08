#!/usr/bin/env python3
# This script matches control rod worths of the Moltres RoddedMaterial to
# results from a more detailed transport code.
#
# usage:
#   controlRodWorthMatcher.py <inputfile> <rodWorthFile>
#
# The file called rodWorthFile can have any number of rows. The last column
# is assumed to be the control rod worth IN RELATIVE REACTIVITY ie change in (k-1)/k
# as compared to the state with no control rods inserted.
# All preceding columns are assumed to belong to control rods 1, 2, 3, all
# the way up to n. This script will find the moltres absorbtion factor for
# each rod that most closely matches reactivities.
#
# NOTE:
# This script is unfinished. I'm putting it into the python directory so
# that others have an idea of the methodology I had in mind to determine
# the rod multiplicative factor for absorbtion for systems with many rods.
#
# The general idea would be to form a residual term that is squared reactivity
# differences between detailed transport (e.g. Serpent) and Moltres' results.
# A Jacobian is formed from perturbing each rod multiplicative factor. Then
# a Newton iteration is done to drive the residual to zero. Repeat until results
# are within uncertainty of the Monte Carlo neutronics. Note that forming the Jacobian
# for this residual should probably happen in parallel on a computing cluster.
#
import numpy
import subprocess
import sys

inputFile = sys.argv[1]
rodWorthFile = sys.argv[2]

if len(sys.argv) > 2:
    raise Exception("Too many command line args.")

with open(inputFile) as infile:
    infileText = infile.read()

with open(rodWorthFile) as rWFile:
    for line in rWFile:
        sline = line.split()
        rodPos, worth = ( float(sline[0]), float(sline[1]) )
