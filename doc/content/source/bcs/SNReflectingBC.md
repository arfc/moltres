# SNReflectingBC

!syntax description /BCs/SNReflectingBC

## Overview

This object applies the reflecting boundary condition for the $S_N$ neutron transport method.
This BC can only be applied to plane boundaries perpendicular to the x-, y-, or z-axis.

!equation
\langle\bm{\Psi}^*,\bm{\Psi}\rangle^+ - \langle\bm{\Psi}^*,\mathbb{B}\bm{\Psi}\rangle^- =
  \begin{cases}
    \sum^G_{g=1}\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d},
    \hat{\Omega}_d\cdot\hat{n}_b\Psi_{g,d}\right),
    & \hat{\Omega}\cdot\hat{n}_b>0,\vec{r}\in\partial\mathcal{D}_s \\
    \sum^G_{g=1}\sum^{N_d}_{d=1}w_d\left(\Psi^*_{g,d},
    \hat{\Omega}_d\cdot\hat{n}_b\Psi_{g,d_r}\right),
    & \hat{\Omega}\cdot\hat{n}_b<0,\vec{r}\in\partial\mathcal{D}_s
  \end{cases},

where

!equation
\hat{n}_b = \text{ boundary unit vector}, \\
\hat{\Omega}_{d_r} = \hat{\Omega}_d - 2(\hat{\Omega}_d\cdot\hat{n}_b)\hat{n}_b = \text{ direction
  of incident neutron travel.}

## Example Input File Syntax

!! Describe and include an example of how to use the SNReflectingBC object.

!syntax parameters /BCs/SNReflectingBC

!syntax inputs /BCs/SNReflectingBC

!syntax children /BCs/SNReflectingBC
