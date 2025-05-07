# SNRodMaterial

!syntax description /Materials/SNRodMaterial

## Overview

This material class inherits all group constant-handling functionality in MoltresSNMaterial and
additionally allows for rod cusping correction using polynomial fitting. Rod cusping occurs when
the control rod boundaries do not intersect perfectly with mesh element interfaces during rod
movement.

This material computes a "corrected rod volume fraction" $V_r'$ using user-provided values as follows:

!equation
V_r' = c V_r^p

where

!equation
p = \text{ cusping correction power factor} \\
c = \text{ cusping correction coefficient}.

## Example Input File Syntax

!! Describe and include an example of how to use the SNRodMaterial object.

!syntax parameters /Materials/SNRodMaterial

!syntax inputs /Materials/SNRodMaterial

!syntax children /Materials/SNRodMaterial
