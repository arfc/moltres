#!/bin/bash

PROJECTDIR=/home/lindsayad/projects_stable

cd $PROJECTDIR/moltres
make clobber
# METHOD=dbg make clobber

cd ../moose
# git pull origin master
git clean -fxd
./scripts/update_and_rebuild_libmesh.sh

cd ../moltres
make -j4
# METHOD=dbg make -j4
