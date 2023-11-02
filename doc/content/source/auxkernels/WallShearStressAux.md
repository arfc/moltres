# WallShearStressAux

!syntax description /AuxKernels/WallShearStressAux

## Overview

This object computes the wall shear stress
$\tau_w=\mu\left(\frac{\partial u_x}{\partial y}\right)_{y=0}$
where $x$ is the direction of fluid flow and $y$ is the direction perpendicular to the wall.
Moltres automatically detects and sets the appropriate directions. This object is valid for
straight walls and non-separated flows only (i.e. no reversed flow along the wall).

## Example Input File Syntax

!! Describe and include an example of how to use the WallShearStressAux object.

!syntax parameters /AuxKernels/WallShearStressAux

!syntax inputs /AuxKernels/WallShearStressAux

!syntax children /AuxKernels/WallShearStressAux
