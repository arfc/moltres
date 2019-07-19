"""
     Python module that takes homogenized XS and prepares them for Moltres

"""
import json
import sys
import numpy as np
from pyne import serpent


class scale_xs:
    """
        Python class that reads in a scale t16 file and organizes the cross
        section data into a numpy dictionary. Currently set up to read an
        arbitrary number of energy groups, an arbitrart number of delayed
        neutron groups, an arbitrary number of identities, an arbitrary
        number of temperature branches, and an arbitrary number of burnups.

        Parameters
        ----------
        xs_filename: str
            Name of file containing collapsed cross section data
        Returns
        ----------
        xs_lib: dict
            A hierarchical dictionary, organized by burnup, id,
            and temperature.
            Currently stores REMXS, FISSXS, NSF, FISSE, DIFFCOEF, RECIPVEL,
            CHI, BETA_EFF, DECAY_CONSTANT and GTRANSFXS.
    """

    def __init__(self, xs_filename):
        with open(xs_filename) as f:
            self.lines = f.readlines()
        self.catch = {
            'Betas': self.t16_line([], True, ['BETA_EFF']),
            'Lambdas': self.t16_line([], True, ['DECAY_CONSTANT']),
            'total-transfer': self.t16_line([1], False, ['REMXS']),
            'fission': self.t16_line([0, 3, 4], False,
                                     ['FISSXS', 'NSF', 'FISSE']
                                     ),
            'chi': self.t16_line([1, 1, 2], False,
                                 ['CHI', 'CHI_D', 'DIFFCOEF']
                                 ),
            'detector': self.t16_line([4], False, ['RECIPVEL']),
            'Scattering cross': self.t16_line([], True, ['GTRANSFXS'])
            }
        XS_entries = ['REMXS', 'FISSXS', 'NSF', 'FISSE', 'DIFFCOEF',
                      'RECIPVEL', 'CHI', 'BETA_EFF', 'DECAY_CONSTANT',
                      'CHI_D', 'GTRANSFXS'
                      ]
        struct = (self.lines[3].split()[:4])
        self.num_burn = int(struct[0])
        self.num_temps = int(struct[1]) + 1
        self.num_uni = int(struct[2])
        self.num_groups = int(struct[3])
        self.xs_lib = {}
        for i in range(self.num_burn):
            self.xs_lib[i] = {}
            for j in range(self.num_uni):
                self.xs_lib[i][j] = {}
                for k in range(self.num_temps):
                    self.xs_lib[i][j][k] = {}
                    for entry in XS_entries:
                        self.xs_lib[i][j][k][entry] = []
        self.get_xs()
        self.fix_xs()

    class t16_line:
        def __init__(self, ind, multi, entry):
            self.index = ind
            self.multi_line_flag = multi
            self.xs_entry = entry

    def fix_xs(self):
        for i in range(self.num_burn):
            for j in range(self.num_uni):
                for k in range(self.num_temps):
                    for ii in range(self.num_groups):
                        start = ii*self.num_groups
                        stop = start + self.num_groups
                        self.xs_lib[i][j][k]["REMXS"][ii] += np.sum(
                            self.xs_lib[i][j][k]["GTRANSFXS"][start:stop])\
                            - self.xs_lib[i][j][k]["GTRANSFXS"][start+ii]
                        if self.xs_lib[i][j][k]["FISSE"][ii] != 0:
                            self.xs_lib[i][j][k]["FISSE"][ii] = (
                                self.xs_lib[i][j][k]["FISSE"][ii] /
                                self.xs_lib[i][j][k]["FISSXS"][ii]
                                )

    def get_xs(self):
        uni = []
        L = 0
        m = 0
        n = 0
        lam_temp = 0
        beta_temp = 0
        for k, line in enumerate(self.lines):
            if 'Identifier' in line:
                val = int(self.lines[k+1].split()[0])
                if val not in uni:
                    uni.extend([val])
                m = uni.index(val)
                self.xs_lib[L][m][n]['BETA_EFF']\
                    .extend(beta_temp)
                self.xs_lib[L][m][n]['DECAY_CONSTANT']\
                    .extend(lam_temp)
            if 'branch no.' in line:
                index = line.find(',')
                L = int(line[index-4:index])
                n = int(line.split()[-1])
            for key in self.catch.keys():
                if key in line:
                    if self.catch[key].multi_line_flag:
                        if (key == 'Betas'):
                            beta_temp = self.get_multi_line_values(k)
                        elif(key == 'Lambdas'):
                            lam_temp = self.get_multi_line_values(k)
                        else:
                            self.xs_lib[L][m][n][self.catch[key].xs_entry[0]]\
                                .extend(
                                    self.get_multi_line_values(k)
                                    )
                    else:
                        for dex, xs in enumerate(self.catch[key].xs_entry):
                            dex = self.catch[key].index[dex]
                            self.xs_lib[L][m][n][xs]\
                                .append(self.get_values(k, dex))

    def get_values(self, k, index):
        val = list(np.array(self.lines[k+1].split()).astype(float))
        return val[index]

    def get_multi_line_values(self, k):
        values = []
        while True:
            val = self.lines[k+1].split()
            k += 1
            for ent in val:
                try:
                    values.append(float(ent))
                except(ValueError):
                    return values


class serpent_xs:
    """
        Python class that reads in a serpent res file and organizes the cross
        section data into a numpy dictionary. Currently set up to read an
        arbitrary number of energy groups, delayed neutron groups, identities,
        temperature branches, and burnups.

        Parameters
        ----------
        xs_filename: str
            Name of file containing collapsed cross section data
        Returns
        ----------
        xs_lib: dict
            A hierarchical dict, organized by burnup, id, and temperature.
            Stores REMXS, FISSXS, NSF, FISSE, DIFFCOEF, RECIPVEL, CHI,
            BETA_EFF, DECAY_CONSTANT and GTRANSFXS.
    """

    def __init__(self, xs_filename):
        data = serpent.parse_res(xs_filename)
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
                    self.xs_lib[i][j][k]["REMXS"] = list(
                        data['INF_REMXS'][index][::2])
                    self.xs_lib[i][j][k]["FISSXS"] = list(
                        data['INF_FISS'][index][::2])
                    self.xs_lib[i][j][k]["NSF"] = list(
                        data['INF_NSF'][index][::2])
                    self.xs_lib[i][j][k]["FISSE"] = list(
                        data['INF_KAPPA'][index][::2])
                    self.xs_lib[i][j][k]["DIFFCOEF"] = list(
                        data['INF_DIFFCOEF'][index][::2])
                    self.xs_lib[i][j][k]["RECIPVEL"] = list(
                        data['INF_INVV'][index][::2])
                    self.xs_lib[i][j][k]["CHI"] = list(
                        data['INF_CHIT'][index][::2])
                    self.xs_lib[i][j][k]["CHI_D"] = list(
                        data['INF_CHID'][index][::2])
                    self.xs_lib[i][j][k]["BETA_EFF"] = list(
                        data['BETA_EFF'][index][::2])
                    self.xs_lib[i][j][k]["DECAY_CONSTANT"] = list(
                        data['LAMBDA'][index][::2])
                    self.xs_lib[i][j][k]["GTRANSFXS"] = list(
                        data['INF_SP0'][index][::2])


def read_input(fin):
    with open(fin) as f:
        lines = f.readlines()
    k = 0
    for k, line in enumerate(lines):
        if '[TITLE]' in line:
            f = open(lines[k+1].split()[0], 'w')
        if '[MAT]' in line:
            mat_dict = {}
            num_mats = int(lines[k+1].split()[0])
            val = lines[k+2].split()
            for i in range(num_mats):
                mat_dict[val[i]] = {'temps': [],
                                    'file': [],
                                    'uni': [],
                                    'burn': [],
                                    'bran': []
                                    }
        if '[BRANCH]' in line:
            tot_branch = int(lines[k+1].split()[0])
            for i in range(tot_branch):
                val = lines[k+2+i].split()
                mat_dict[val[0]]['temps'].extend(
                    [int(val[1])])
                mat_dict[val[0]]['file'].extend(
                    [int(val[2])])
                mat_dict[val[0]]['burn'].extend(
                    [int(val[3])])
                mat_dict[val[0]]['uni'].extend(
                    [int(val[4])])
                mat_dict[val[0]]['bran'].extend(
                    [int(val[5])])

        if 'FILES' in line:
            num_files = int(lines[k+1].split()[0])
            files = {}
            for i in range(num_files):
                XS_in, XS_t = lines[k+2+i].split()
                if 'scale' in XS_t:
                    files[i] = scale_xs(XS_in)
                elif 'serpent'in XS_t:
                    files[i] = serpent_xs(XS_in)
                else:
                    raise("XS data not understood\n \
                          Please use: scale or serpent")
    out_dict = {}
    for entry in mat_dict:
        out_dict[entry] = {'temp': mat_dict[entry]['temps']}
        for i, t in enumerate(mat_dict[entry]['temps']):
            L = mat_dict[entry]['file'][i] - 1
            m = mat_dict[entry]['burn'][i] - 1
            n = mat_dict[entry]['uni'][i] - 1
            p = mat_dict[entry]['bran'][i] - 1
            out_dict[entry][str(t)] = files[L].xs_lib[m][n][p]
    f.write(json.dumps(out_dict, sort_keys=True, indent=4))


if len(sys.argv) < 2:
    raise("Input file not provided")
read_input(sys.argv[1])
