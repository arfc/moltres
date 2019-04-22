MoltresXS
=====

MoltresXS is a python tool designed for converting Serpent and SCALE 
cross sections to a Moltres readable format.


## Install

MoltresXS relies on the pyne python packages.
To install pyne please follow the instructions at
https://pyne.io/install/index.html.

## Use

To use MoltresXS run the following commands

```bash
python MoltresXS.py XS.inp
```


## XS.inp

The cross section input file is structured such that various materials and 
branches can be condensed into a single HDF5 file. The file is broken into four sections,
[TITLE], [MAT], [BRANCH], and [FILES]. 
An example XS input file can be found in moltres/tutorial/PinCell 

#[TITLE]
Specifies the name of HDF5 file to be created

#[MAT]
Species the number of materials being created.

Lists the names of the different materials

#[BRANCH]
Specifies the number of branches being created.

Lists the name of the material, the temperature of the branch, the 
index of the file containing the branch, the index of the burnup step 
containing the branch, the index of the universe containing the material and
the index of the cross section branch.

#[FILES]
Specifies the number of files to be read.
Lists the path/name of the file followed by
the clasification of the file, (1=scale, 2=serpent)

