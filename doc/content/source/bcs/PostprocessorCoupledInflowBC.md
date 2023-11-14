# PostprocessorCoupledInflowBC

!syntax description /BCs/PostprocessorCoupledInflowBC

## Overview

This object adds the inflow boundary condition for the delayed neutron precursor equation if
[CoupledScalarAdvection](/CoupledScalarAdvection.md) is used (i.e. integration by parts and
divergence theorem). The weak form for this boundary condition is:

!equation
\int_{\partial V} \psi C_{inlet} \vec{u}\cdot\hat{n}\ dS

where $C_{inlet}$ is the precursor concentration at the inlet and defined by the `postprocessor`
input parameter.

## Example Input File Syntax

!! Describe and include an example of how to use the PostprocessorCoupledInflowBC object.

!syntax parameters /BCs/PostprocessorCoupledInflowBC

!syntax inputs /BCs/PostprocessorCoupledInflowBC

!syntax children /BCs/PostprocessorCoupledInflowBC
