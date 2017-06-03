#!/bin/bash

# function append_material {
#     echo "\n" >> $1
#     echo "[Materials]\n" >> $1
#     echo "  [./const]\n" >> $1
#     echo "    type = GenericConstantMaterial\n" >> $1
#     echo "    prop_names = 'mu rho'\n" >> $1
#     echo "    prop_values = '1 1'\n" >> $1
#     echo "  [../]\n" >> $1
#     echo "[]\n" >> $1
# }

# read -rd '' VAR <<EOM
# [Materials]
#   [./mat]
#     type = GenericConstantMaterial
#     prop_names = 'mu rho'
#     prop_values = '1 1'
#   [../]
# []
# EOM

# echo $VAR
# echo "$VAR" > log.txt

# # grep --include=\*.i -rnl "INSMomentum" | awk '{ print $1 }' | sed 's/:.*//g' | xargs -I '{}' echo '{}'
# # grep --include=\*.i -rnl "INSMomentum" | xargs -I '{}' echo "[Materials]" >> '{}'
# grep --include=\*.i -rnl "INSMomentum" | xargs -I {} sh -c 'cat >> "$1" <<- EOM
# [Materials]
#   [./mat]
#     type = GenericConstantMaterial
#     prop_names = 'mu rho'
#     prop_values = '1 1'
#   [../]
# []
# EOM
# echo "$VAR" >> "$1"' -- {}

for file in $(grep --include=\*.i -rnl "INSMomentum"); do
    echo "" >> $file
    echo "[Materials]" >> $file
    echo "  [./const]" >> $file
    echo "    type = GenericConstantMaterial" >> $file
    echo "    prop_names = 'mu rho'" >> $file
    echo "    prop_values = '1 1'" >> $file
    echo "  [../]" >> $file
    echo "[]" >> $file
done
