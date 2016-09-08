#!/bin/bash

git clean -fxd
cd moose
git clean -fxd
./scripts/update_and_rebuild_libmesh.sh
cd ..
make -j4
