# WallDistanceAux

!syntax description /AuxKernels/WallDistanceAux

## Overview

This object computes the minimum distance of each mesh node to the nearest wall node. The wall
distance calculation may be slightly inaccurate for unstructured or skewed structured meshes as
illustrated on the [Turbulence Modeling Resource](https://turbmodels.larc.nasa.gov/spalart.html)
webpage.

## Example Input File Syntax

!! Describe and include an example of how to use the WallDistanceAux object.

!syntax parameters /AuxKernels/WallDistanceAux

!syntax inputs /AuxKernels/WallDistanceAux

!syntax children /AuxKernels/WallDistanceAux
