#!/bin/bash

OLD=$1
NEW=$2

path=$PWD
new_path="${path/src/include}"

cp $OLD".C" $NEW".C"
sed -i "s/$OLD/$NEW/g" $NEW".C"

cd $new_path
cp $OLD".h" $NEW".h"
sed -i "s/$OLD/$NEW/g" $NEW".h"
sed -i "s/${OLD^^}/${NEW^^}/g" $NEW".h"
