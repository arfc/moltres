# ScalarAdvectionArtDiff

!syntax description /Kernels/ScalarAdvectionArtDiff

## Overview

This object adds an isotropic artificial diffusion term where:

!equation
\gamma = \frac{|\vec{u}|h}{2D}

and

!equation
\tau=\frac{1}{\tanh\gamma}-\frac{1}{\gamma}.

This scheme is not recommended for most simulations because it tends to produce overly diffusive
results.

## Example Input File Syntax

!! Describe and include an example of how to use the ScalarAdvectionArtDiff object.

!syntax parameters /Kernels/ScalarAdvectionArtDiff

!syntax inputs /Kernels/ScalarAdvectionArtDiff

!syntax children /Kernels/ScalarAdvectionArtDiff
