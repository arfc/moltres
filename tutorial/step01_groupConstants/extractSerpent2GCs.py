#!/usr/bin/env python3
# This script extracts group constants from Serpent 2. It should be able to do all of the work, no
# need to specify how many energy groups, or anything like that. Also, this could be imported into
# other python scripts if needed, maybe for parametric studies.

import os
import numpy as np
import argparse
import subprocess

try:
    from pyne import serpent as sss
except ImportError as err:
    print("Sorry, looks like PyNE wasn't able to be loaded. Please check your installation.")
    print("Maybe you only have it for python 2.")
    print( err )
    quit()

def makePropertiesDir(inmats, outdir, filebase, mapFile, unimapFile, serp1=False, fromMain=False):
    """ Takes in a mapping from branch names to material temperatures,
    then makes a properties directory.
    Serp1 means that the group transfer matrix is transposed."""

    if serp1:
        raise NotImplementedError("C'mon, just get serpent 2!")

    if not os.path.isdir(outdir):
        os.mkdir(outdir)
    print("Making properties for materials:")
    print(inmats)
    coeList = dict([(mat,sss.parse_coe(mat+'.coe')) for mat in inmats])

    # the constants moltres looks for:
    goodStuff = ['BETA_EFF','CHI','DECAY_CONSTANT','DIFF_COEF','FISSE','GTRANSFXS','NSF','RECIPVEL','REMXS','FISSXS']
    goodMap   = dict([(thing, 'INF_'+thing) for thing in goodStuff])

    # the name for the group transfer XS matrix is different in serpent
    # and moltres, so this gets fixed:
    goodMap['GTRANSFXS'] = 'INF_SP0' # scattering + production in 0 legendre moment
    goodMap['BETA_EFF'] = 'BETA_EFF'
    goodMap['CHI'] = 'INF_CHIP' # include production. chip, chit, or chid???
    goodMap['DECAY_CONSTANT']='LAMBDA'
    goodMap['RECIPVEL'] = 'INF_INVV'
    goodMap['DIFF_COEF'] = 'INF_DIFFCOEF'
    goodMap['FISSE'] = 'INF_KAPPA'
    goodMap['FISSXS'] = 'INF_FISS'

    # map material names to universe names from serpent
    with open(unimapFile) as fh:
        uniMap = []
        for line in fh:
            uniMap.append(tuple(line.split()))
    # this now maps material names to serpent universes
    uniMap = dict(uniMap)

    branch2TempMapping = open(mapFile)
    for line in branch2TempMapping:

        item, temp = tuple(line.split())
        for mat in inmats:
            if mat in item:
                currentMat = mat
                break
        else:
            print('Considered materials: {}'.format(inmats))
            raise Exception('Couldnt find a material corresponding to branch {}'.format(item))

        for coefficient in goodStuff:
            with open(outdir+'/'+filebase+currentMat+'_'+coefficient+'.txt', 'a') as fh:
                strData = coeList[currentMat][1][uniMap[currentMat]][item]["rod0"][goodMap[coefficient]]
                if coefficient == 'DECAY_CONSTANT' or coefficient == 'BETA_EFF':
                    # some additional formatting is needed here
                    strData = strData[1:7]
                strData = ' '.join([str(dat) for dat in strData]) if isinstance(strData,list) else strData
                fh.write(str(temp)+' '+strData)
                fh.write('\n')

    return None


if __name__ == '__main__':

    # make it act like a nice little terminal program
    parser = argparse.ArgumentParser(description='Extracts Serpent 2 group constants, and puts them in a directory suitable for moltres.')
    parser.add_argument('outDir', metavar='o', type=str, nargs=1, help='name of directory to write properties to.')
    parser.add_argument('fileBase', metavar='f', type=str, nargs=1, help='File base name to give moltres')
    parser.add_argument('mapFile' , metavar='b', type=str, nargs=1, help='File that maps branches to temperatures')
    parser.add_argument('universeMap', metavar='u', type=str, nargs=1, help='File that maps material names to serpent universe')
    parser.add_argument('materials', metavar='m', type=str, nargs='+', help='list of moltres material names')
    parser.add_argument('--serp1', dest='serp1', action='store_true', help='use this flag for serpent 1 group transfer matrices')
    parser.set_defaults(serp1=False)

    args = parser.parse_args()

    # these are unpacked, so it fails if they werent passed to the script
    inmats = args.materials
    outdir = args.outDir[0]
    fileBase = args.fileBase[0]
    mapfile = args.mapFile[0]
    unimapfile = args.universeMap[0]

    makePropertiesDir(inmats, outdir, fileBase, mapfile,unimapfile, serp1=args.serp1, fromMain = True)

    print("Successfully made property files in directory {}.".format(outdir))
