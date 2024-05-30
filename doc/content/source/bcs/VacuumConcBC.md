# VacuumConcBC

!syntax description /BCs/VacuumConcBC

## Overview

This object adds the $\phi_g+\alpha D_g\hat{n}\cdot\nabla\phi_g = 0$ vacuum boundary
condition of the multigroup neutron diffusion equations. The weak form after applying integration
by parts to the neutron diffusion term ([GroupDiffusion](/GroupDiffusion.md)) is:

!equation
-\int_{\partial V}\psi D_g\nabla\phi_g\cdot\hat{n}\ dS =
\int_{\partial V}\psi\frac{\phi_g}{\alpha}\ dS

The value of $\alpha$ varies depending on the formulation selected using the `vacuum_bc_type`
parameter.
1. Marshak: $\alpha=2$
2. Mark: $\alpha=\sqrt{3}$
3. Milne: $\alpha=3\times 0.710446$

## Example Input File Syntax

!! Describe and include an example of how to use the VacuumConcBC object.

!syntax parameters /BCs/VacuumConcBC

!syntax inputs /BCs/VacuumConcBC

!syntax children /BCs/VacuumConcBC
