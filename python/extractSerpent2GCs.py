#!/usr/bin/env python3
# This script extracts group constants from Serpent 2. It should be
# able to do all of the work, no need to specify how many energy
# groups, or anything like that. Also, this could be imported into
# other python scripts if needed, maybe for parametric studies.
import os
import numpy as np
import argparse
import subprocess
import serpentTools as sT


def makePropertiesDir(
        outdir,
        filebase,
        mapFile,
        secbranchFile,
        unimapFile,
        serp1=False,
        fromMain=False):
    """ Takes in a mapping from branch names to material temperatures,
    then makes a properties directory.
    Serp1 means that the group transfer matrix is transposed."""

    if serp1:
        raise NotImplementedError("C'mon, just get serpent 2!")

    if not os.path.isdir(outdir):
        os.mkdir(outdir)

    # the constants moltres looks for:
    goodStuff = ['BETA_EFF', 'Chit', 'Chid', 'lambda', 'Diffcoef', 'Kappa',
                 'Sp0', 'Nsf', 'Invv', 'Remxs', 'Fiss', 'Nubar', 'Flx']
    goodMap = dict([(thing, 'inf' + thing) for thing in goodStuff])
    goodMap['BETA_EFF'] = 'betaEff'
    goodMap['lambda'] = 'lambda'

    # map material names to universe names from serpent
    with open(unimapFile) as fh:
        uniMap = []
        for line in fh:
            uniMap.append(tuple(line.split()))
    # this now maps material names to serpent universes
    uniMap = dict(uniMap)

    # list of material names
    inmats = list(uniMap.keys())

    print("Making properties for materials:")
    print(inmats)
    coeList = dict([(mat, sT.read(mat + '.coe')) for mat in inmats])

    # secondary branch burnup steps
    secBranch = []
    with open(secbranchFile) as bf:
        for line in bf:
            secBranch.append(line)

    # primary branch to temp mapping
    branch2TempMapping = open(mapFile)

    # Check if calculation uses 6 neutron precursor groups.
    # This prevents writing of excess zeros. Check if any
    # entries in the 7th and 8th group precursor positions
    # are nonzero, if so, use 8 groups.
    use8Groups = False
    for line in branch2TempMapping:
        item, temp = tuple(line.split())
        for mat in inmats:
            if mat in item:
                currentMat = mat
                break
        if secBranch:
            for branch in secBranch:
                strData = coeList[currentMat].branches[
                    item, branch].universes[
                        uniMap[currentMat], 0, 0, None].gc[goodMap['BETA_EFF']]
                strData = strData[1:9]
                if np.any(strData[-2:] != 0.0):
                    use8Groups = True
        else:
            strData = coeList[currentMat].branches[item].universes[
                uniMap[currentMat], 0, 0, None].gc[goodMap['BETA_EFF']]
            strData = strData[1:9]
            if np.any(strData[-2:] != 0.0):
                use8Groups = True

    # Now loop through a second time
    branch2TempMapping.close()
    branch2TempMapping = open(mapFile)

    for line in branch2TempMapping:

        item, temp = tuple(line.split())
        for mat in inmats:
            if mat in item:
                currentMat = mat
                break
        else:
            print('Considered materials: {}'.format(inmats))
            raise Exception(
                'Couldnt find a material corresponding to branch {}'.format(
                    item))

        try:
            if secBranch:
                for branch in secBranch:
                    for coefficient in goodStuff:
                        with open(outdir + '/' + filebase + '_' + currentMat +
                                  '_' + branch + '_' + coefficient.upper() +
                                  '.txt', 'a') as fh:
                            if coefficient == 'lambda' or \
                                    coefficient == 'BETA_EFF':
                                strData = coeList[currentMat].branches[
                                    item, branch].universes[
                                        uniMap[currentMat], 0, 0, None].gc[
                                            goodMap[coefficient]]
                                # some additional formatting is needed here
                                strData = strData[1:9]

                                # Cut off group 7 and 8 precursor params in 6
                                # group calcs
                                if not use8Groups:
                                    strData = strData[0:6]
                            else:
                                strData = coeList[currentMat].branches[
                                    item, branch].universes[
                                        uniMap[currentMat], 0, 0, None].infExp[
                                            goodMap[coefficient]]
                            strData = ' '.join(
                                [str(dat) for dat in strData]) if isinstance(
                                    strData, np.ndarray) else strData
                            fh.write(str(temp) + ' ' + strData)
                            fh.write('\n')

            else:
                for coefficient in goodStuff:
                    with open(outdir + '/' + filebase + '_' + currentMat +
                              '_' + coefficient.upper() + '.txt', 'a') as fh:
                        if coefficient == 'lambda' or \
                                coefficient == 'BETA_EFF':
                            strData = coeList[currentMat].branches[
                                item].universes[
                                    uniMap[currentMat], 0, 0, None].gc[
                                        goodMap[coefficient]]
                            # some additional formatting is needed here
                            strData = strData[1:9]

                            # Cut off group 7 and 8 precursor params in 6
                            # group calcs
                            if not use8Groups:
                                strData = strData[0:6]
                        else:
                            strData = coeList[currentMat].branches[
                                item].universes[
                                    uniMap[currentMat], 0, 0, None].infExp[
                                        goodMap[coefficient]]
                        strData = ' '.join(
                            [str(dat) for dat in strData]) if isinstance(
                                strData, np.ndarray) else strData
                        fh.write(str(temp) + ' ' + strData)
                        fh.write('\n')

        except KeyError:
            print(secBranch)
            raise Exception('Check your mapping and secondary branch files.')

if __name__ == '__main__':

    # make it act like a nice little terminal program
    parser = argparse.ArgumentParser(
        description='Extracts Serpent 2 group constants, \
            and puts them in a directory suitable for moltres.')
    parser.add_argument('outDir', metavar='o', type=str, nargs=1,
                        help='name of directory to write properties to.')
    parser.add_argument('fileBase', metavar='f', type=str,
                        nargs=1, help='File base name to give moltres')
    parser.add_argument(
        'mapFile',
        metavar='b',
        type=str,
        nargs=1,
        help='File that maps branches to temperatures')
    parser.add_argument(
        'secbranchFile',
        metavar='s',
        type=str,
        nargs=1,
        help='File that lists secondary Serpent branch names')
    parser.add_argument(
        'universeMap',
        metavar='u',
        type=str,
        nargs=1,
        help='File that maps material names to serpent universe')
    parser.add_argument(
        '--serp1',
        dest='serp1',
        action='store_true',
        help='use this flag for serpent 1 group transfer matrices')
    parser.set_defaults(serp1=False)

    args = parser.parse_args()

    # these are unpacked, so it fails if they werent passed to the script
    outdir = args.outDir[0]
    fileBase = args.fileBase[0]
    mapFile = args.mapFile[0]
    secbranchFile = args.secbranchFile[0]
    unimapFile = args.universeMap[0]

    makePropertiesDir(outdir, fileBase, mapFile, secbranchFile,
                      unimapFile, serp1=args.serp1, fromMain=True)

    print("Successfully made property files in directory {}.".format(outdir))
