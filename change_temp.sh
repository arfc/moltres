#!/bin/bash

cd "/home/lindsayad/projects/moltres/src/kernels"
find -name "*.C" -exec sed -i "s/\(addCoupledVar(\"temperature\",\) [0-9]*/\1 $1/g" {} +
