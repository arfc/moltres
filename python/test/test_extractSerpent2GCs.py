import sys
import os
import shutil
import numpy as np
import argparse
import serpentTools as sT
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from extractSerpent2GCs import *


def test_makePropertiesDir():
    """Testing extractSerpent2GCs.py with Serpent 2 XS data, compared
    with the expected JSON file in the gold directory
    """
    outdir = 'xs'                   # Name of directory to hold xs data
    fileBase = 'msfr'               # Prefix for xs data filenames
    mapFile = 'tempMapping.txt'     # Map of branches to temperatures
    secbranchFile = 'sec.txt'       # Secondary branches, if any
    unimapFile = 'unimapFile.txt'   # Map of materials to universes

    temp_map = ['fuel0 900 \n', 'fuel1 1200 \n',
                'blanket0 900 \n', 'blanket1 1200 \n']
    uni_map = ['fuel 1 \n', 'blanket 3 \n']

    # Write txt files required as input
    file1 = open(mapFile, 'w')
    file1.writelines(temp_map)
    file1.close()

    file2 = open(secbranchFile, 'w')
    file2.close()

    file3 = open(unimapFile, 'w')
    file3.writelines(uni_map)
    file3.close()

    # Rename .coe file to match material names
    shutil.copyfile('MSFR_base.coe', 'fuel.coe')
    shutil.copyfile('MSFR_base.coe', 'blanket.coe')

    # Run makePropertiesDir
    makePropertiesDir(outdir, fileBase, mapFile, secbranchFile,
                      unimapFile)

    # Delete input files
    os.remove(mapFile)
    os.remove(secbranchFile)
    os.remove(unimapFile)
    os.remove('fuel.coe')
    os.remove('blanket.coe')

    # Material and xs names
    materials = ['fuel', 'blanket']
    xs_list = ['BETA_EFF', 'CHID', 'CHIP', 'DIFFCOEF', 'FISS', 'FLX',
               'INVV', 'KAPPA', 'LAMBDA', 'NSF', 'NUBAR', 'REMXS', 'SP0']

    # Check every xs file against the expected files in ./gold/xs/
    for mat in materials:
        for xs in xs_list:
            path = outdir + '/' + fileBase + '_' + mat + '_' + xs + '.txt'
            data = list(open(path, 'r'))
            expected = list(open('gold/' + path, 'r'))
            assert data == expected
            os.remove(path)     # Delete xs file after check
    os.rmdir(outdir)        # Delete empty xs directory
