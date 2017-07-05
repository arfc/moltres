#!/usr/bin/env python

import numpy as np
import subprocess

xsecs = ["FLUX", "REMXS", "FISSXS", "NUBAR", "NSF", "FISSE", "DIFFCOEF", \
         "RECIPVEL", "CHI", "GTRANSFXS", "BETA_EFF", "DECAY_CONSTANT"]
num_groups = 2
num_precursor_groups = 8
xsec_dict = {"FLUX" : num_groups, "REMXS" : num_groups, "FISSXS" : num_groups, \
             "NUBAR" : num_groups, "NSF" : num_groups, "FISSE" : num_groups, \
             "DIFFCOEF" : num_groups, "RECIPVEL" : num_groups, "CHI" : num_groups, \
             "GTRANSFXS" : num_groups * num_groups, "BETA_EFF" : num_precursor_groups, \
             "DECAY_CONSTANT" : num_precursor_groups}
materials = ["fuel", "mod"]

for material in materials:
    write_file_name =  subprocess.call(["touch", "write_" + material])
    file_base = "msr2g_Th_U_two_mat_homogenization_dens_func_" + material + "_data_func_of_" + material + "_temp"
    write_file = open("write_" + material, 'r+')
    for xsec in xsecs:
        file_name = file_base + "_" + xsec + ".txt"
        data = np.loadtxt(file_name)
        temperature = data[:, 0]
        A = np.vstack([temperature, np.ones(len(temperature))]).T
        for k in range(xsec_dict[xsec]):
            dep_var = data[:, k+1]
            m, c = np.linalg.lstsq(A, dep_var)[0]
            write_file.write(str(m) + " " + str(c))
            write_file.write('\n')
    write_file.close()
