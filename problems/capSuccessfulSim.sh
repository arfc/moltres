#!/bin/bash

echo "What's the INSTRING?"
read INSTRING
echo "What's the DESCRIPTOR?"
read DESCRIPTOR
echo "What's the GEOFILE?"
read GEOFILE
INSTRING=$INSTRING
ROOTSTRING=$INSTRING
OUTSTRING=$ROOTSTRING"_"$DESCRIPTOR
GEOFILE=$GEOFILE

mkdir "MooseGold/$OUTSTRING"
cp $INSTRING".i" "MooseGold/$OUTSTRING/Input.i"
cp $INSTRING"_out.e" "MooseGold/$OUTSTRING/Output.e"
cp $INSTRING"_out.csv" "MooseGold/$OUTSTRING/Output.csv"
cp $GEOFILE "MooseGold/$OUTSTRING/$GEOFILE"
cp -t "MooseGold/$OUTSTRING/" log.txt *interp*.txt


drive push -no-clobber ~/gdrive/MooseGold <<<Y
# drive push -no-clobber ~/gdrive/Pictures <<<Y
