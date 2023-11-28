# CoupledOutflowBC

!syntax description /BCs/CoupledOutflowBC

## Overview

This object adds the outflow boundary condition for the delayed neutron precursor equation if
[CoupledScalarAdvection](CoupledScalarAdvection.md) is used (i.e. integration by parts and
divergence theorem). The weak form for this boundary
condition is:

!equation
\int_{\partial V}\psi C_i\vec{u}\cdot\hat{n}\ dS.

## Example Input File Syntax

!! Describe and include an example of how to use the CoupledOutflowBC object.

!syntax parameters /BCs/CoupledOutflowBC

!syntax inputs /BCs/CoupledOutflowBC

!syntax children /BCs/CoupledOutflowBC
