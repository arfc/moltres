# VacuumConcBC

!syntax description /BCs/VacuumConcBC

## Overview

This object adds the $\frac{\phi}{4}-\frac{D_g}{2}\hat{n}\cdot\nabla\phi_g = 0$ vacuum boundary
condition of the multigroup neutron diffusion equations. The weak form after applying integration
by parts to the neutron diffusion term ([GroupDiffusion](/GroupDiffusion.md)) is:

!equation
\int_{\partial V}\psi D_g\nabla\phi_g\cdot\hat{n}\ dS = \int_{\partial V}\psi\frac{\phi_g}{2}\ dS

## Example Input File Syntax

!! Describe and include an example of how to use the VacuumConcBC object.

!syntax parameters /BCs/VacuumConcBC

!syntax inputs /BCs/VacuumConcBC

!syntax children /BCs/VacuumConcBC
