#!/bin/bash

INSTRING="nts-temp-delayed"
ROOTSTRING=$INSTRING
OUTSTRING=$ROOTSTRING"_cammi_3d_nts_only_inverse_pow_method"
GEOFILE="cylinder.geo"

mkdir "MooseGold/$OUTSTRING"
cp $INSTRING".i" "MooseGold/$OUTSTRING/Input.i"
cp $INSTRING"_out.e" "MooseGold/$OUTSTRING/Output.e"
cp $GEOFILE "MooseGold/$OUTSTRING/Geometry.geo"
# cp "log.txt" "MooseLogs/log_"$OUTSTRING"_gold.txt"

drive push -no-clobber ~/gdrive/MooseGold
drive push -no-clobber ~/gdrive/Pictures
