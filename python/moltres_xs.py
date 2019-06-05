"""
     Python Function that takes homoginezed cross sections and prepares them for MOLTRES

"""
import json
import io
import sys
import numpy as np
from pyne import serpent


class scale_xs:
    """
        Python class that reads in a scale t16 file and organizes the cross section data
        into a numpy dictionary. Currently set up to read an arbitrary number of energy groups,
        an arbitrart number of delayed neutron groups, an arbitrary number of identities,
        an arbitrary number of temperature branches, and an arbitrary number of burnups.

        Parameters
        ----------
        xs_filename: str
            Name of file containing collapsed cross section data
        Returns
        ----------
        xs_lib: dict
            A hierarchical dictionary, organized by burnup, id, and temperature.
            Currently stores REMXS, FISSXS, NSF, FISSE, DIFFCOEF, RECIPVEL, CHI, BETA_EFF, DECAY_CONSTANT and
            GTRANSFXS.
    """

    def __init__(self, xs_filename):
        with io.open(xs_filename) as f:
            self.lines = f.readlines()
        self.value_flags = {#'Identifier':  {'index': [0],     'multi_line_flag' : False , 'xs_entry': ['null']},
                    'Fuel temperatures':   {'index': [],      'multi_line_flag' : True , 'xs_entry': ['temp']},
                    'Nominal':             {'index': [2],     'multi_line_flag' : False , 'xs_entry': ['temp']},
                    'Betas':               {'index': [],      'multi_line_flag' : True , 'xs_entry': ['BETA_EFF']},
                    'Lambdas':             {'index': [],      'multi_line_flag' : True , 'xs_entry': ['DECAY_CONSTANT']},
                    'total-transfer':      {'index': [1],     'multi_line_flag' : False , 'xs_entry': ['REMXS']},
                    'fission' :            {'index': [0,3,4], 'multi_line_flag' : False , 'xs_entry': ['FISXS','NSF','FISSE']},
                    'chi':                 {'index': [1,2],   'multi_line_flag' : False , 'xs_entry': ['CHI','DIFFCOEF']},
                    'detector':            {'index': [4],     'multi_line_flag' : False , 'xs_entry': ['RECIPVEL']},
                    'Scattering cross':    {'index': [],      'multi_line_flag' : True , 'xs_entry': ['GTRANSFXS']}
                    }
        XS_entries = ['remxs', 'fisxs', 'nsf', 'fisse', 'diffcoef', 'recipvel', 'chi', 'beta_eff', 'decay_constant','chi_d','gtransfxs']
        struct = (self.lines[3].split()[:4])
        self.num_burn, self.num_temps,self.num_uni, self.num_groups = int(
            struct[0]), int(struct[1]), int(struct[2]), int(struct[3])
        if self.num_temps == 0:
            self.num_temps = 1
        self.xs_lib = {}
        for i in range(self.num_burn):
            self.xs_lib[i] = {}
            for j in range(self.num_uni):
                self.xs_lib[i][j] = {}
                for k in range(self.num_temps):
                    self.xs_lib[i][j][k] = {}
                    for entry in XS_entries:
                        self.xs_lib[i][j][k][entry.upper()] = []
        self.get_xs()
        self.fix_xs()
    def fix_xs(self):
        for i in range(self.num_burn):
            for j in range(self.num_uni):
                for k in range(self.num_temps):
                    for ii in range(self.num_groups):
                        self.xs_lib[i][j][k]["REMXS"][ii] += np.sum(
                            self.xs_lib[i][j][k]["GTRANSFXS"][ii*self.num_groups:ii*self.num_groups+self.num_groups])-self.xs_lib[i][j][k]["GTRANSFXS"][ii*self.num_groups+ii]
                        if self.xs_lib[i][j][k]["FISSE"][ii] != 0:
                            self.xs_lib[i][j][k]["FISSE"][ii] = self.xs_lib[i][j][k]["FISSE"][ii]/self.xs_lib[i][j][k]["FISXS"][ii] 
        

    def get_xs(self):
        r_l,m_l,m = 0,0,0
        uni = []
        for k,line in enumerate(self.lines):
            if 'Identifier' in line:
                val = int(self.lines[k+1].split()[0])
                if val not in uni:
                    uni.extend([val])
                m = uni.index(val)
            if 'branch no.' in line:
                r_l += 1
                l = r_l // self.num_burn
                n=int(line.split()[-1])
            if "'" in line:
                for key in self.value_flags.keys():
                    if key in line:
                        if self.value_flags[key]['multi_line_flag']:
                            self.xs_lib[l][m][n][self.value_flags[key]['xs_entry'][0]].extend(self.get_multi_line_values(k))
                        else:
                            for index,xs_key in enumerate(self.value_flags[key]['xs_entry']):
                                index=self.value_flags[key]['index'][index]
                                self.xs_lib[l][m][n][xs_key].extend([self.get_values(k,index)])
            
    def get_values(self,k,index):
        val = list(np.array(self.lines[k+1].split()).astype(float))
        return val[index]

    def get_multi_line_values(self,k):
        values = []
        while True:
            val = self.lines[k+1].split()
            k += 1
            for ent in val:
                try:
                    values.extend([float(ent)])
                except(ValueError):
                    return values
        


class serpent_xs:
    """
        Python class that reads in a serpent res file and organizes the cross section data
        into a numpy dictionary. Currently set up to read an arbitrary number of energy groups,
        an arbitrart number of delayed neutron groups, an arbitrary number of identities,
        an arbitrary number of temperature branches, and an arbitrary number of burnups.

        Parameters
        ----------
        xs_filename: str
            Name of file containing collapsed cross section data
        Returns
        ----------
        xs_lib: dict
            A hierarchical dictionary, organized by burnup, id, and temperature.
            Currently stores REMXS, FISSXS, NSF, FISSE, DIFFCOEF, RECIPVEL, CHI, BETA_EFF, DECAY_CONSTANT and
            GTRANSFXS.
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
                    self.xs_lib[i][j][k]["REMXS"] = list(data['INF_REMXS'][index][::2])
                    self.xs_lib[i][j][k]["FISSXS"] = list(data['INF_FISS'][index][::2])
                    self.xs_lib[i][j][k]["NSF"] = list(data['INF_NSF'][index][::2])
                    self.xs_lib[i][j][k]["FISSE"] = list(data['INF_KAPPA'][index][::2])
                    self.xs_lib[i][j][k]["DIFFCOEF"] = list(data['INF_DIFFCOEF'][index][::2])
                    self.xs_lib[i][j][k]["RECIPVEL"] = list(data['INF_INVV'][index][::2])
                    self.xs_lib[i][j][k]["CHI"] = list(data['INF_CHIT'][index][::2])
                    self.xs_lib[i][j][k]["CHI_D"] = list(data['INF_CHID'][index][::2])
                    self.xs_lib[i][j][k]["BETA_EFF"] = list(data['BETA_EFF'][index][::2])
                    self.xs_lib[i][j][k]["DECAY_CONSTANT"] = list(data['LAMBDA'][index][::2])
                    self.xs_lib[i][j][k]["GTRANSFXS"] = list(data['INF_SP0'][index][::2])


def READ_INPUT(fin):
    with io.open(fin) as f:
        lines = f.readlines()
    k = 0
    while k < len(lines):
        line = lines[k]
        if '[TITLE]' in line:
            f = open(lines[k+1].split()[0], 'w')
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
    out_dict = {}
    for entry in mat_dict:
        print(entry)
        out_dict[entry]={ 'temp' : list(mat_dict[entry]['temps'])}
        for i in range(len(mat_dict[entry]['temps'])):
            out_dict[entry][str(mat_dict[entry]['temps'][i])] = files[mat_dict[entry]['file'][i]-1].xs_lib[mat_dict[entry]['burn'][i]-1][mat_dict[entry]
                                                                                                ['uni'][i]-1][int(mat_dict[entry]['bran'][i]-1)]
    f.write(json.dumps(out_dict, sort_keys=True, indent=4))
    quit()
if len(sys.argv) < 2:
    raise("Input file not provided")
READ_INPUT(sys.argv[1])
