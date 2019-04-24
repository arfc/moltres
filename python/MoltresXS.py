"""
     Python Function that takes homoginezed cross sections and prepares them for MOLTRES

"""

import io
import sys
import numpy as np
import h5py
from pyne import serpent


class scale_xs:
    """
        Python class that reads in a scale t16 file and organizes the cross section data
        into a numpy dictionary. Currently set up to read an arbitrary number of energy groups,
        an arbitrart number of delayed neutron groups, an arbitrary number of identities,
        an arbitrary number of temperature branches, and an arbitrary number of burnups.

        Parameters
        ----------
        FileIn: str
            Name of file containing collapsed cross section data
        Returns
        ----------
        xs_lib: dict
            A hierarchical dictionary, organized by burnup, id, and temperature.
            Currently stores REMXS, FISSXS, NSF, FISSE, DIFFCOEF, RECIPVEL, CHI, BETA_EFF, DECAY_CONSTANT and
            GTRANSFXS.
    """

    def __init__(self, FileIn):
        with io.open(FileIn) as f:
            lines = f.readlines()
        struct = (lines[3].split()[:4])

        num_burn, num_uni, num_groups = int(
            struct[0]), int(struct[2]), int(struct[3])
        ids = np.zeros(num_uni)
        temps = np.array([])
        burn = np.zeros([])
        remxs = np.array([])
        fissxs = np.array([])
        nsf = np.array([])
        fissE = np.array([])
        difcoef = np.array([])
        invV = np.array([])
        chi = np.array([])
        transxs = np.array([])
        beta = np.array([])
        decay = np.array([])

        i = 0
        k = 0
        while k < len(lines):
            line = lines[k]
            if 'Identifier' in line:
                val = int(lines[k+1].split()[0])
                k += 1
                if not any(val == ids):
                    ids[i] = val
                    i += 1
            if 'Fuel temperatures' in line:
                while 1:
                    val = lines[k+1].split()
                    k += 1
                    for j in range(len(val)):
                        temps = np.append(temps, float(val[j]))
                    if "'" in lines[k+1]:
                        break

            if 'Nominal' in line:
                val = lines[k+2].split()
                k += 2
                temps = np.append(temps, float(val[0]))
            if 'Burnups' in line:
                while 1:
                    val = lines[k+1].split()
                    k += 1
                    for j in range(len(val)):
                        burn = np.append(burn, float(val[j]))
                    if "'" in lines[k+1]:
                        break
            if 'Betas' in line:
                while 1:
                    val = lines[k+1].split()
                    k += 1
                    for j in range(len(val)):
                        beta = np.append(beta, float(val[j]))
                    if "'" in lines[k+1]:
                        break
                if not 'num_delay_groups' in locals():
                    num_delay_groups = len(beta)
            if 'Lambdas' in line:
                while 1:
                    val = lines[k+1].split()
                    k += 1
                    for j in range(len(val)):
                        decay = np.append(decay, float(val[j]))
                    if "'" in lines[k+1]:
                        break
            if 'total-transfer' in line:
                val = lines[k+1].split()
                remxs = np.append(remxs, float(val[1]))

                val = lines[k+3].split()
                fissxs = np.append(fissxs, float(val[0]))
                nsf = np.append(nsf, float(val[3]))
                fissE = np.append(nsf, float(val[4]))

                val = lines[k+5].split()
                difcoef = np.append(difcoef, float(val[2]))
                chi = np.append(chi, float(val[1]))

                val = lines[k+7].split()
                invV = np.append(invV, float(val[4]))
                k += 7
            if 'Scattering cross' in line:
                while 1:
                    val = lines[k+1].split()
                    k += 1
                    for j in range(len(val)):
                        transxs = np.append(transxs, float(val[j]))
                    if "'" in lines[k+1]:
                        break
            k += 1
        num_temps = len(temps)
        if num_temps == 0:
            num_temps = 1
            temps = 600*np.ones(1)
        self.xs_lib = {}
        for i in range(num_burn):
            self.xs_lib[i] = {}
            for j in range(num_uni):
                self.xs_lib[i][j] = {}
                for k in range(num_temps):
                    self.xs_lib[i][j][k] = {}
                    index = i*(num_temps*num_uni*num_groups)+k * \
                        (num_groups*num_uni)+j*num_groups
                    self.xs_lib[i][j][k]["REMXS"] = remxs[index:index+num_groups]
                    self.xs_lib[i][j][k]["FISSXS"] = fissxs[index:index+num_groups]
                    self.xs_lib[i][j][k]["NSF"] = nsf[index:index+num_groups]
                    self.xs_lib[i][j][k]["FISSE"] = np.divide(fissE[index:index+num_groups], fissxs[index:index+num_groups], out=np.zeros_like(
                        fissE[index:index+num_groups]), where=fissxs[index:index+num_groups] != 0)
                    self.xs_lib[i][j][k]["DIFFCOEF"] = difcoef[index:index+num_groups]
                    self.xs_lib[i][j][k]["RECIPVEL"] = invV[index:index+num_groups]
                    self.xs_lib[i][j][k]["CHI"] = chi[index:index+num_groups]
                    self.xs_lib[i][j][k]["CHI_D"] = chi[index:index+num_groups]*0
                    self.xs_lib[i][j][k]["CHI_D"][0] = 1
                    index = i*(num_temps*num_uni*num_delay_groups) + \
                        k*(num_uni*num_delay_groups)+j*num_delay_groups
                    self.xs_lib[i][j][k]["BETA_EFF"] = beta[index:index +
                                                            num_delay_groups]
                    self.xs_lib[i][j][k]["DECAY_CONSTANT"] = decay[index:index+num_delay_groups]
                    index = i*(num_temps*num_uni*num_groups*num_groups)+k * \
                        (num_groups*num_groups*num_uni)+j*num_groups*num_groups
                    self.xs_lib[i][j][k]["GTRANSFXS"] = transxs[index:index +
                                                                num_groups*num_groups]
                    for ii in range(num_groups):
                        self.xs_lib[i][j][k]["REMXS"][ii] += np.sum(
                            self.xs_lib[i][j][k]["GTRANSFXS"][ii*num_groups:ii*num_groups+num_groups])-self.xs_lib[i][j][k]["GTRANSFXS"][ii*num_groups+ii]


class serpent_xs:
    """
        Python class that reads in a serpent res file and organizes the cross section data
        into a numpy dictionary. Currently set up to read an arbitrary number of energy groups,
        an arbitrart number of delayed neutron groups, an arbitrary number of identities,
        an arbitrary number of temperature branches, and an arbitrary number of burnups.

        Parameters
        ----------
        FileIn: str
            Name of file containing collapsed cross section data
        Returns
        ----------
        xs_lib: dict
            A hierarchical dictionary, organized by burnup, id, and temperature.
            Currently stores REMXS, FISSXS, NSF, FISSE, DIFFCOEF, RECIPVEL, CHI, BETA_EFF, DECAY_CONSTANT and
            GTRANSFXS.
    """

    def __init__(self, FileIn):
        data = serpent.parse_res(FileIn)
        try:
            num_burn = len(np.unique(data['BURNUP'][:][0]))
        except(KeyError):
            num_burn = 1
        num_uni = len(np.unique(data['GC_UNIVERSE_NAME']))
        num_temps = int(len(data['GC_UNIVERSE_NAME'])/(num_uni*num_burn))
        self.xs_lib = {}
        for i in range(num_burn):
            self.xs_lib[i] = {}
            for j in range(num_uni):
                self.xs_lib[i][j] = {}
                for k in range(num_temps):
                    self.xs_lib[i][j][k] = {}
                    index = i*(num_uni)+k*(num_burn*num_uni)+j
                    self.xs_lib[i][j][k]["REMXS"] = data['INF_REMXS'][index][::2]
                    self.xs_lib[i][j][k]["FISSXS"] = data['INF_FISS'][index][::2]
                    self.xs_lib[i][j][k]["NSF"] = data['INF_NSF'][index][::2]
                    self.xs_lib[i][j][k]["FISSE"] = data['INF_KAPPA'][index][::2]
                    self.xs_lib[i][j][k]["DIFFCOEF"] = data['INF_DIFFCOEF'][index][::2]
                    self.xs_lib[i][j][k]["RECIPVEL"] = data['INF_INVV'][index][::2]
                    self.xs_lib[i][j][k]["CHI"] = data['INF_CHIT'][index][::2]
                    self.xs_lib[i][j][k]["CHI_D"] = data['INF_CHID'][index][::2]
                    self.xs_lib[i][j][k]["BETA_EFF"] = data['BETA_EFF'][index][::2]
                    self.xs_lib[i][j][k]["DECAY_CONSTANT"] = data['LAMBDA'][index][::2]
                    self.xs_lib[i][j][k]["GTRANSFXS"] = data['INF_SP0'][index][::2]


def XS_OUT(fout, xs_dir, mat, temp):
    for k, v in xs_dir.items():
        fout.create_dataset(mat+"/"+str(int(temp))+"/"+k, data=v)


def TEMP_OUT(fout, mat, temps):
    fout.create_dataset(mat+"/temp/", data=temps)


def READ_INPUT(fin):
    with io.open(fin) as f:
        lines = f.readlines()
    k = 0
    while k < len(lines):
        line = lines[k]
        if '[TITLE]' in line:
            f = h5py.File(lines[k+1].split()[0], 'w')
            k += 1

        if '[MAT]' in line:
            mat_dict = {}
            num_mats = int(lines[k+1].split()[0])
            val = lines[k+2].split()
            for i in range(num_mats):
                mat_dict[val[i]] = {'temps': np.array([]),
                                    'file': np.array([]),
                                    'uni': np.array([]),
                                    'burn': np.array([]),
                                    'bran': np.array([])
                                    }
            k += 2
        if '[BRANCH]' in line:
            tot_branch = int(lines[k+1].split()[0])
            for i in range(tot_branch):
                val = lines[k+2+i].split()
                mat_dict[val[0]]['temps'] = np.append(
                    int(val[1]), mat_dict[val[0]]['temps'])
                mat_dict[val[0]]['file'] = np.append(
                    int(val[2]), mat_dict[val[0]]['file'])
                mat_dict[val[0]]['burn'] = np.append(
                    int(val[3]), mat_dict[val[0]]['burn'])
                mat_dict[val[0]]['uni'] = np.append(
                    int(val[4]), mat_dict[val[0]]['uni'])
                mat_dict[val[0]]['bran'] = np.append(
                    int(val[5]), mat_dict[val[0]]['bran'])
            k += 1+tot_branch
        if 'FILES' in line:
            num_files = int(lines[k+1].split()[0])
            files = {}
            for i in range(num_files):
                XS_in, XS_t = lines[k+2+i].split()
                if int(XS_t) == 1:
                    files[i] = scale_xs(XS_in)
                elif int(XS_t) == 2:
                    files[i] = serpent_xs(XS_in)
                else:
                    raise("XS data not understood\n 1=scale \n 2=serpent")
            k += 1+num_files
        k += 1
    for entry in mat_dict:
        TEMP_OUT(f, entry, mat_dict[entry]['temps'])
        for i in range(len(mat_dict[entry]['temps'])):
            XS_OUT(f, files[mat_dict[entry]['file'][i]-1].xs_lib[mat_dict[entry]['burn'][i]-1][mat_dict[entry]
                                                                                               ['uni'][i]-1][int(mat_dict[entry]['bran'][i]-1)], entry, mat_dict[entry]['temps'][i])


if len(sys.argv) < 2:
    raise("Input file not provided")
READ_INPUT(sys.argv[1])
