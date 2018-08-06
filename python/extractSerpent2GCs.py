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


def parse_coe(coefile, grabB1=True):
    """ Used to parse output from serpent 2's ability to branch problem
    variables in order to generate group constants at several states.

    See the serpent wiki at:
    http://serpent.vtt.fi/mediawiki/index.php/Automated_burnup_sequence

    Example:
        say that you apply this to a serpent output with fuel temperature
        changes named fuel0 through fuel14. Then, control rod positions
        were altered with branches rod0 through rod4. In total, this
        would make 75 sets of different group constants since all
        combinations are covered. To access INF_KINF with
        (ie k_infinity with no B1 corrections) in uni 2
        fuel0, rod0 at day 0 of burnup, you'd use:

        coe[1]         ["2"]     ["fuel0"]["rod0"]["INF_KINF"]
        #   ^ dep step  ^ uni ID  ^branches       ^ parameter name

        all parameters serpent gives are available by default


    Parameters
    ----------
    coefile : str or file-like object
        Path to *.coe file or a *.coe file handle.
    grabB1 : bool
        whether to include results from the B1 corrected spectrum
        if group constants were generated on full-core, set this to false
        to save memory.

    Returns
    -------
    coe : dict
        Nested dictionary of the parsed coe file. First dimension key is burnup
        index (1-indexed). Next is the universe group constants were generated
        in. Then, branch names represent the keys, in the order that serpent
        printed them. On the final dimension, the parameters returned by
        serpent make up the remaining keys.

        note: B1 is used for leakage-corrected group constants generated on an
        infinite lattice. Don't use these values otherwise.

    """
    if isinstance(coefile, str):
        f = open(coefile, 'r')
        fromFh = False  # not from file handle
    else:
        f = coefile
        fromFh = True  # from a file handle

    coe = {}
    layers = None

    while True:

        # this line describes branch and coe indices
        try:
            l = next(f).split()
        except StopIteration:
            break
        _, numbranch, coeIndex, nTot, nUni = tuple([int(item) for item in l])

        # this line describes what branch you're on
        l = next(f).split()
        # init list to hold branch names if it is still None
        if layers is None:
            layers = [[] for i in range(int(l[0]))]
        for i, branch in enumerate(l[1:]):
            # append if not seen before
            if branch not in layers[i]:
                layers[i].append(branch)
        theseBranches = l[1:]

        next(f)
        l = next(f).split()
        BUi = int(l[1])  # index of BU step
        if BUi not in coe.keys():
            coe[BUi] = {}

        # ok, should be at a totally new set of data now,
        # so, let's gather all params.
        for iUni in range(nUni):

            l = next(f).split()
            uni = l[0]
            if uni not in coe[BUi].keys():
                coe[BUi][uni] = {}
            nPara = int(l[1])  # num of params to follow

            thisCoe = coe[BUi][uni]
            # delve through the branches, reassigning the thisCoe reference
            # at each successive level
            for i, branch in enumerate(theseBranches):
                if branch not in thisCoe.keys():
                    thisCoe[branch] = {}
                if i == len(theseBranches)-1:
                    break
                thisCoe = thisCoe[branch]

            names = []
            values = []  # should all be numeric in nature
            for i in range(nPara):
                # build from lists to avoid frequently rehashing dicts
                l = next(f).split()
                name = l[0]
                # check if B1
                # also, yeah 'and' could be used, but bool checking
                # is faster than string comparison, so that's why to nest
                if not grabB1:
                    if name[0:2] == 'B1':
                        continue
                value = l[2:]
                names.append(name)
                if len(values) == 1:
                    values.append(float(value[0]))
                else:
                    values.append([float(item) for item in value])

            thisCoe[branch] = dict(zip(names, values))

    if not fromFh:
        # if file handle wasnt passed, close what this function opened
        f.close()

    return coe


def makePropertiesDir(outdir, filebase, mapFile, secbranchFile, unimapFile, serp1=False, fromMain=False):
    """ Takes in a mapping from branch names to material temperatures,
    then makes a properties directory.
    Serp1 means that the group transfer matrix is transposed."""

    if serp1:
        raise NotImplementedError("C'mon, just get serpent 2!")

    if not os.path.isdir(outdir):
        os.mkdir(outdir)
    

    # the constants moltres looks for:
    goodStuff = ['BETA_EFF','CHIT','LAMBDA','DIFFCOEF','KAPPA','SP0','NSF','INVV','REMXS','FISS','NUBAR','FLX']
    goodMap   = dict([(thing, 'INF_'+thing) for thing in goodStuff])
    goodMap['BETA_EFF'] = 'BETA_EFF'
    goodMap['LAMBDA'] = 'LAMBDA'

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
    coeList = dict([(mat, parse_coe(mat+'.coe')) for mat in inmats])
    
    # secondary branch burnup steps
    secBranch = []
    with open(secbranchFile) as bf:
        for line in bf:
            secBranch.append(line)
    
    # primary branch to temp mapping
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


        try:
            if not secBranch:
                for coefficient in goodStuff:
                    with open(outdir+'/'+filebase+'_'+currentMat+'_'+coefficient+'.txt', 'a') as fh:
                        strData = coeList[currentMat][1][uniMap[currentMat]][item][goodMap[coefficient]]
                        if coefficient == 'LAMBDA' or coefficient == 'BETA_EFF':
                            # some additional formatting is needed here
                            strData = strData[1:7]
                        strData = ' '.join([str(dat) for dat in strData]) if isinstance(strData,list) else strData
                        fh.write(str(temp)+' '+strData)
                        fh.write('\n')

            else:
                for branch in secBranch:
                    for coefficient in goodStuff:
                        with open(outdir+'/'+filebase+'_'+currentMat+'_'+branch+'_'+coefficient+'.txt', 'a') as fh:
                            strData = coeList[currentMat][1][uniMap[currentMat]][item][branch][goodMap[coefficient]]
                            if coefficient == 'LAMBDA' or coefficient == 'BETA_EFF':
                                # some additional formatting is needed here
                                strData = strData[1:7]
                            strData = ' '.join([str(dat) for dat in strData]) if isinstance(strData,list) else strData
                            fh.write(str(temp)+' '+strData)
                            fh.write('\n')
        except KeyError:
            print(secBranch)
            raise Exception('Check your mapping and secondary branch files.')

    return None


if __name__ == '__main__':

    # make it act like a nice little terminal program
    parser = argparse.ArgumentParser(description='Extracts Serpent 2 group constants, and puts them in a directory suitable for moltres.')
    parser.add_argument('outDir', metavar='o', type=str, nargs=1, help='name of directory to write properties to.')
    parser.add_argument('fileBase', metavar='f', type=str, nargs=1, help='File base name to give moltres')
    parser.add_argument('mapFile' , metavar='b', type=str, nargs=1, help='File that maps branches to temperatures')
    parser.add_argument('secbranchFile', metavar='s', type=str, nargs=1, help='File that lists secondary Serpent branch names')
    parser.add_argument('universeMap', metavar='u', type=str, nargs=1, help='File that maps material names to serpent universe')
    parser.add_argument('--serp1', dest='serp1', action='store_true', help='use this flag for serpent 1 group transfer matrices')
    parser.set_defaults(serp1=False)

    args = parser.parse_args()

    # these are unpacked, so it fails if they werent passed to the script
    outdir = args.outDir[0]
    fileBase = args.fileBase[0]
    mapFile = args.mapFile[0]
    secbranchFile = args.secbranchFile[0]
    unimapFile = args.universeMap[0]

    makePropertiesDir(outdir, fileBase, mapFile, secbranchFile, unimapFile, serp1=args.serp1, fromMain = True)

    print("Successfully made property files in directory {}.".format(outdir))
