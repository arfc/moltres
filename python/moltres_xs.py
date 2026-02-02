#!/usr/bin/env python3
# This script extracts homogenized group constants from Serpent 2 or SCALE into
# a JSON data file to be used with MoltresJsonMaterial.
import json
import sys
import argparse
import numpy as np
import h5py
import openmc
import importlib


class openmc_mgxslib:
    def __init__ (self, stpt_file, summ_file, json_path):
        
        """
             Reads OpenMC Statepoint and Summary Files that contain mgxs.Library() MGXS Tallies and builds a .json.  
             Is set up to read only 1 Statepoint and Summary as of now, will add multi-file capbilities soon. 
             
             mgxs.Library() is the most optimized, efficient, and user-friendly way to generate MGXS Tallies
             This function requires the user to have a properly setup mgxs.Library()
             
             Is currently defined for an arbitrary number of domains, materials, energy groups, and delayed groups. 
            
            Requirements
            ----------
            mgxs.Libary().mgxs_types
                The types configured in the users mgxs.Library() must at least have the required mgxs_types 
                that are given in the __init__ function. 
            mgxs.Library().legendre_order = 3
                Legendre Order must be 3 to properly compute and format GTRANSFXS.
                (I do not know if this is a true requirement, but it matches the godiva.json GTRANSFXS numbers)
            Parameters
            ----------
            stpt_file: str
                Name of OpenMC Statepoint HDF5 file containing collapsed cross section
                data
            summ_file: str
                Name of OpenMC Summary file associated with the Statepoint file
            json_path: str
                Name of .json file and path where the json_store will be dumped. 
            Returns
            ----------
            json_store: dict
                A hierarchical dictionary that stores all MGXS Tallies requested/required. 
                Organized by material and temperature. 
                Contains: BETA_EFF, CHI_T, CHI_D, CHI_P, DECAY_CONSTANT, DIFFCOEF, FISSXS,
                FISSE, GTRANSFXS, NSF, and RECIPVEL. 
                
        """
        
        self.stpt_file = str(stpt_file)
        self.summ_file = str(summ_file)
        self.json_path = str(json_path)
        self.required_mgxs_types = {
            "beta", 
            "chi", 
            "chi-prompt", 
            "chi-delayed", 
            "decay-rate", 
            "diffusion-coefficient", 
            "fission", 
            "kappa-fission", 
            "consistent nu-scatter matrix",
            "nu-fission", 
            "inverse-velocity",}
        
    def build_json(self, mgxslib):
        self.json_store = {}
        
        statepoint = openmc.StatePoint(self.stpt_file, autolink = False)
        summary = openmc.Summary(self.summ_file)
        statepoint.link_with_summary(summary)
        if not hasattr(mgxslib, "load_from_statepoint") or not hasattr(mgxslib, "build_hdf5_store"):
            raise TypeError("Passed Library Object invalid, must be mgxs.Library()")

        mgxslib.load_from_statepoint(statepoint)

        material_id_to_name = {mat.id: mat.name for mat in summary.materials}
        material_id_to_object = {mat.id: mat for mat in summary.materials}
        
        rename_mgxs = {
            "beta": "BETA_EFF",
            "chi": "CHI_T",
            "chi-prompt": "CHI_P",
            "chi-delayed": "CHI_D",
            "decay-rate": "DECAY_CONSTANT",
            "diffusion-coefficient": "DIFFCOEF",
            "fission": "FISSXS",
            "inverse-velocity": "RECIPVEL",
        }
        
        mgxslib.build_hdf5_store()
        
        with h5py.File("mgxs/mgxs.h5", "r") as f:
            for domain_type in f:
                for domain_id in f[domain_type]:
                    if int(domain_id) not in material_id_to_object:
                        raise KeyError(f"Domain ID {domain_id} not found in materials summary")
                    domain_name = material_id_to_name.get(int(domain_id), str(domain_id))
                    self.json_store[domain_name] = {}
                    
                    mat = material_id_to_object.get(int(domain_id))
                    temps =mat.temperature if isinstance(mat.temperature, list) else [mat.temperature]
                    for temp in temps:
                        self.json_store[domain_name][temp] = {}
                        for mgxs_type in self.required_mgxs_types:
                            if mgxs_type not in f[domain_type][domain_id]:
                                raise KeyError(f"MGXS type '{mgxs_type}' not found for material '{domain_name}'")
                            mgxs_data = f[domain_type][domain_id][mgxs_type]
                            arr = mgxs_data["average"][:]
                            
                            if mgxs_type == "kappa-fission":
                                fission_data = f[domain_type][domain_id]["fission"]["average"][:]
                                fission_data = np.array(fission_data)
                                kappa_data = np.array(arr)
                                
                                safe_arr = np.zeros_like(fission_data)
                                mask = fission_data != 0
                                safe_arr[mask] = (kappa_data[mask] / fission_data[mask]) * 1e-6
                                arr = safe_arr
                            
                            if mgxs_type == "consistent nu-scatter matrix":
                                P0 = arr[:, :, 0]
                                arr = P0
                                
                            if mgxs_type == "chi-delayed":
                                chi_d = arr.sum(axis = 0)
                                chi_d /= chi_d.sum()
                                arr = chi_d
                                
                            if mgxs_type == "beta":
                                beta = arr.sum(axis = 1)
                                arr = beta

                            if arr.ndim > 1:
                                arr = arr.flatten()
                                
                            arr = arr.tolist()
                            mgxs_key = rename_mgxs.get(mgxs_type, mgxs_type)
                            
                            if mgxs_type == "consistent nu-scatter matrix":
                                mgxs_key = "GTRANSFXS"
                            elif mgxs_type == "kappa-fission":
                                mgxs_key = "FISSE"
                            elif mgxs_type == "nu-fission":
                                mgxs_key = "NSF" # New OpenMC nu-fission tally already comes flux weighted. 
                            
                            self.json_store[domain_name][temp][mgxs_key] = arr
                    self.json_store[domain_name]["temp"] = temps               
        return self.json_store
    
    def dump_json(self):
        if not self.json_store:
            raise ValueError("JSON Store is empty, mgxs.Library() did not load correctly or build_json() did not run.")

        with open(self.json_path, 'w') as f:
            json.dump(self.json_store , f, indent=4)

class openmc_xs:
    """
    Reads OpenMC h5 statepoint file and organizes the cross section
    date into a dictionary. Currently set up to read an
    arbitrary number of energy groups, an arbitrary number of delayed
    neutron groups, an arbitrary number of identities, and an arbitrary
    number of temperature branches.

    Parameters
    ----------
    xs_filename: str
        Name of OpenMC Statepoint HDF5 file containing collapsed cross section
        data
    file_num: int
        File number
    xs_summary: str
        Name of OpenMC Summary file associated with the Statepoint file
    Returns
    ----------
    xs_lib: dict
        A hierarchical dictionary, organized by burnup, id, and temperature.
        Currently stores REMXS, FISSXS, NSF, FISSE, DIFFCOEF, RECIPVEL,
        CHI, BETA_EFF, DECAY_CONSTANT and GTRANSFXS.
    """

    def __init__(self, xs_filename, file_num, xs_summary):
        sp = openmc.StatePoint(xs_filename, autolink=False)
        summary = openmc.Summary(xs_summary)
        sp.link_with_summary(summary)
        domain_dict = openmc_ref_modules[file_num].domain_dict
        num_burn = 1
        num_branch = 0
        num_uni = []
        for k in sp.filters:
            v = sp.filters[k]
            if isinstance(v, openmc.filter.MaterialFilter):
                num_uni.append(v.bins[0])
            elif isinstance(v, openmc.filter.CellFilter):
                num_uni.append(v.bins[0])
        self.xs_lib = {}
        for i in range(num_burn):
            self.xs_lib[i] = {}
            for j in num_uni:
                k = num_branch
                self.xs_lib[i][j - 1] = {}
                self.xs_lib[i][j - 1][k] = {}
                self.xs_lib[i][j - 1][k]["BETA_EFF"] = self.mgxs_tallies(
                    sp, domain_dict[j]["beta"])
                self.xs_lib[i][j - 1][k]["CHI_T"] = self.mgxs_tallies(
                    sp, domain_dict[j]["chi"])
                self.xs_lib[i][j - 1][k]["CHI_P"] = self.mgxs_tallies(
                    sp, domain_dict[j]["chiprompt"])
                self.xs_lib[i][j - 1][k]["CHI_D"] = self.mgxs_tallies(
                    sp, domain_dict[j]["chidelayed"])
                self.xs_lib[i][j - 1][k]["DECAY_CONSTANT"] = self.mgxs_tallies(
                    sp, domain_dict[j]["decayrate"])
                self.xs_lib[i][j - 1][k]["DIFFCOEF"] = self.get_diffcoeff(
                    sp, domain_dict[j]["diffusioncoefficient"])
                self.xs_lib[i][j - 1][k]["FISSE"] = self.get_fisse(
                    sp, domain_dict[j]["fissionxs"],
                    domain_dict[j]["kappafissionxs"])
                self.xs_lib[i][j - 1][k]["GTRANSFXS"] = self.get_nu_scatter(
                    sp, domain_dict[j]["nuscattermatrix"])
                self.xs_lib[i][j - 1][k]["SPN"] = self.get_scatter_pn(
                    sp, domain_dict[j]["nuscattermatrix"])
                self.xs_lib[i][j - 1][k]["NSF"] = self.get_nsf(sp, j)
                self.xs_lib[i][j - 1][k]["RECIPVEL"] = self.mgxs_tallies(
                    sp, domain_dict[j]["inversevelocity"])
                self.xs_lib[i][j - 1][k]["FISSXS"] = self.mgxs_tallies(
                    sp, domain_dict[j]["fissionxs"])
                self.xs_lib[i][j - 1][k]["TOTXS"] = self.get_totxs(
                    sp, domain_dict[j]["totalxs"])
                self.xs_lib[i][j - 1][k]["REMXS"] = self.get_remxs(
                    sp,
                    domain_dict[j]["totalxs"],
                    domain_dict[j]["nuscattermatrix"])
        return

    def mgxs_tallies(self, sp, tally):
        """Returns list of tally values for each energy group

        Parameters
        ----------
        sp: openmc.Statepoint
        tally: OpenMC mgxs tally object

        Returns
        -------
        list
            list of tally values for each energy group
        """

        tally.load_from_statepoint(sp)
        return list(tally.get_pandas_dataframe()["mean"])

    def get_diffcoeff(self, sp, diffcoef):
        """Returns list of diffusion coefficient values for each energy group

        Parameters
        ----------
        sp: openmc.Statepoint
        diffcoef: OpenMC mgxs.DiffusionCoefficient object

        Returns
        -------
        list
            list of diffusion coefficient values for each energy group
        """

        diffcoef.load_from_statepoint(sp)
        diffcoef_df = diffcoef.get_pandas_dataframe()
        return list(diffcoef_df["mean"])

    def get_fisse(self, sp, fissionxs, kappa):
        """Returns list of average deposited fission energy values for each
        energy group

        Parameters
        ----------
        sp: openmc.Statepoint
        fissionxs: OpenMC mgxs.FissionXS object
        kappa: OpenMC mgxs.KappaFissionXS object

        Returns
        -------
        list
            list of average deposited fission energy values for each energy
            group
        """

        fissionxs.load_from_statepoint(sp)
        kappa.load_from_statepoint(sp)
        fissionxs_df = fissionxs.get_pandas_dataframe()
        kappa_df = kappa.get_pandas_dataframe()
        fisse = kappa_df["mean"] / fissionxs_df["mean"] * 1e-6
        fisse = np.array(fisse)
        fisse[np.isnan(fisse)] = 0
        return list(fisse)

    def get_nu_scatter(self, sp, nu_scatter_matrix):
        """Returns scatter xs matrix values for each energy group.
        The matrix is flattened into a list. It is taken from the P0 scatter
        matrix xs with neutron multiplication from (n,xn) reactions.

        Parameters
        ----------
        sp: openmc.Statepoint
        nu_scatter_matrix: OpenMC mgxs.ScatterMatrixXS object

        Returns
        -------
        list
            list of P0 scatter xs matrix values for each energy group
        """

        nu_scatter_matrix.load_from_statepoint(sp)
        nu_scatter_matrix_df = nu_scatter_matrix.get_pandas_dataframe()
        return list(nu_scatter_matrix_df.loc[
            nu_scatter_matrix_df["legendre"] == "P0"]["mean"])

    def get_scatter_pn(self, sp, nu_scatter_matrix):
        """Returns PN scatter xs matrix values for each energy group.
        The matrix is flattened into a list. It is taken from the PN scatter
        matrix xs with neutron multiplication from (n,xn) reactions.

        Parameters
        ----------
        sp: openmc.Statepoint
        nu_scatter_matrix: OpenMC mgxs.ScatterMatrixXS object

        Returns
        -------
        list
            list of PN scatter xs matrix values for each energy group
        """

        nu_scatter_matrix.load_from_statepoint(sp)
        nu_scatter_matrix_df = nu_scatter_matrix.get_pandas_dataframe()
        num_groups = len(list(nu_scatter_matrix_df.loc[
            (nu_scatter_matrix_df["group in"] == 1) &
            (nu_scatter_matrix_df["legendre"] == "P0")]["mean"]))
        num_pn = len(list(nu_scatter_matrix_df.loc[
            (nu_scatter_matrix_df["group in"] == 1) &
            (nu_scatter_matrix_df["group out"] == 1)]["mean"]))
        final_matrix = np.array(nu_scatter_matrix_df["mean"])
        final_matrix.shape = (num_groups**2, num_pn)
        final_matrix = final_matrix.T
        return final_matrix.tolist()

    def get_nsf(self, sp, index):
        """Returns fission neutron production xs values for each energy group.
        It is calculated by dividing OpenMC's nu-fission by flux.
        dividing

        Parameters
        ----------
        sp: openmc.Statepoint
        index: int
            file index

        Returns
        -------
        list
            list of fission neutron production xs values for each energy group
        """

        tally = sp.get_tally(name=str(index) + " tally")
        df = tally.get_pandas_dataframe()
        df_flux = np.array(df.loc[df["score"] == "flux"]["mean"])
        df_nu_fission = df.loc[df["score"] == "nu-fission"]
        nu_fission = list(np.array(df_nu_fission["mean"]) / df_flux)
        nu_fission.reverse()
        return nu_fission

    def get_remxs(self, sp, totalxs, nu_scatter_matrix):
        """Returns removal xs values for each energy group. It is calculated by
        subtracting the within-group scatter xs value with neutron
        multiplication from (n, xn) reactions from the total xs value.

        Parameters
        ----------
        sp: openmc.Statepoint
        totalxs: OpenMC mgxs.TotalXS object
        nu_scatter_matrix: OpenMC mgxs.ScatterMatrixXS object

        Returns
        -------
        list
            list of total xs values for each energy group
        """

        totalxs.load_from_statepoint(sp)
        nu_scatter_matrix.load_from_statepoint(sp)
        totalxs_df = totalxs.get_pandas_dataframe()
        nu_scatter_matrix_df = nu_scatter_matrix.get_pandas_dataframe()
        group_nums = list(totalxs_df["group in"])
        remxs = []
        for i in group_nums:
            totxs = float(
                totalxs_df.loc[totalxs_df["group in"] == i]["mean"].iloc[0])
            nu_self_scatter = float(
                nu_scatter_matrix_df.loc[
                    (nu_scatter_matrix_df["group in"] == i) &
                    (nu_scatter_matrix_df["group out"] == i) &
                    (nu_scatter_matrix_df["legendre"] == "P0")]["mean"].iloc[0]
                )
            remxs.append(totxs - nu_self_scatter)
        return remxs

    def get_totxs(self, sp, totalxs):
        """Returns total xs values for each energy group.

        Parameters
        ----------
        sp: openmc.Statepoint
        totalxs: OpenMC mgxs.TotalXS object

        Returns
        -------
        list
            list of total xs values for each energy group
        """

        totalxs.load_from_statepoint(sp)
        totalxs_df = totalxs.get_pandas_dataframe()
        return list(totalxs_df["mean"])

    def generate_openmc_tallies_xml(energy_groups, delayed_groups, domains,
                                    domain_ids, tallies_file):
        """
        Users should use this function to generate the OpenMC tallies file
        for group constant generation.

        Parameters
        ----------
        energy_groups: list
            list of energy group edges (must include all edges)
        delayed_groups: list
            list of number of delayed neutron groups
        domains: list
            list of openmc domains, these can be openmc.Materials or
            openmc.Cells
        domain_ids: list
            list of openmc domain ids
        tallies_file: openmc.Tallies
            an initialized openmc tallies object

        Returns
        -------
        domain_dict: dict
            dictionary containing initialized tallies

        """

        import openmc
        import warnings
        import openmc.mgxs as mgxs
        version = float(openmc.__version__[2:])
        if version < 13.2:
            raise Exception("moltres_xs.py is compatible with OpenMC " +
                            "version 0.13.2 or later only.")
        elif version > 14.0:
            warnings.warn("moltres_xs.py has not been tested for OpenMC " +
                          "versions newer than 0.14.0.")

        groups = mgxs.EnergyGroups(group_edges=energy_groups)
        big_energy_group = [energy_groups[0], energy_groups[-1]]
        big_group = mgxs.EnergyGroups(group_edges=big_energy_group)
        energy_filter = openmc.EnergyFilter(energy_groups)
        domain_dict = {}
        for id in domain_ids:
            domain_dict[id] = {}
        for domain, id in zip(domains, domain_ids):
            domain_dict[id]["beta"] = mgxs.Beta(
                domain=domain,
                energy_groups=big_group,
                delayed_groups=delayed_groups,
                name=str(id) + "_beta")
            domain_dict[id]["chi"] = mgxs.Chi(
                domain=domain, energy_groups=groups, name=str(id) + "_chi")
            domain_dict[id]["chiprompt"] = mgxs.Chi(
                domain=domain, energy_groups=groups,
                name=str(id) + "_chiprompt", prompt=True)
            domain_dict[id]["chidelayed"] = mgxs.ChiDelayed(
                domain=domain, energy_groups=groups,
                name=str(id) + "_chidelayed")
            domain_dict[id]["decayrate"] = mgxs.DecayRate(
                domain=domain,
                energy_groups=big_group,
                delayed_groups=delayed_groups,
                name=str(id) + "_decayrate")
            domain_dict[id]["diffusioncoefficient"] = \
                mgxs.DiffusionCoefficient(
                    domain=domain,
                    energy_groups=groups,
                    name=str(id) +
                    "_diffusioncoefficient")
            domain_dict[id]["nuscattermatrix"] = mgxs.ScatterMatrixXS(
                domain=domain, energy_groups=groups,
                name=str(id) + "_nuscattermatrix", nu=True)
            domain_dict[id]["nuscattermatrix"].correction = None
            domain_dict[id]["nuscattermatrix"].formulation = 'consistent'
            domain_dict[id]["nuscattermatrix"].legendre_order = 3
            domain_dict[id]["inversevelocity"] = mgxs.InverseVelocity(
                domain=domain, energy_groups=groups,
                name=str(id) + "_inversevelocity")
            domain_dict[id]["fissionxs"] = mgxs.FissionXS(
                domain=domain, energy_groups=groups,
                name=str(id) + "_fissionxs")
            domain_dict[id]["kappafissionxs"] = mgxs.KappaFissionXS(
                domain=domain, energy_groups=groups,
                name=str(id) + "_kappafissionxs")
            domain_dict[id]["totalxs"] = mgxs.TotalXS(
                domain=domain, energy_groups=groups,
                name=str(id) + "_totalxs")
            domain_dict[id]["tally"] = openmc.Tally(name=str(id) + " tally")
            if isinstance(domain, openmc.Material):
                domain_dict[id]["filter"] = openmc.MaterialFilter(domain)
            elif isinstance(domain, openmc.Cell):
                domain_dict[id]["filter"] = openmc.CellFilter(domain)
            else:
                domain_dict[id]["filter"] = openmc.MeshFilter(domain)
            domain_dict[id]["tally"].filters = [
                domain_dict[id]["filter"],
                energy_filter]
            domain_dict[id]["tally"].scores = [
                "nu-fission",
                "flux"]
            tallies_file += domain_dict[id]["beta"].tallies.values()
            tallies_file += domain_dict[id]["chi"].tallies.values()
            tallies_file += domain_dict[id]["chiprompt"].tallies.values()
            tallies_file += domain_dict[id]["chidelayed"].tallies.values()
            tallies_file += domain_dict[id]["decayrate"].tallies.values()
            tallies_file += domain_dict[id]["diffusioncoefficient"] \
                .tallies.values()
            tallies_file += domain_dict[id]["nuscattermatrix"].tallies.values()
            tallies_file += domain_dict[id]["inversevelocity"].tallies.values()
            tallies_file += domain_dict[id]["fissionxs"].tallies.values()
            tallies_file += domain_dict[id]["kappafissionxs"].tallies.values()
            tallies_file += domain_dict[id]["totalxs"].tallies.values()
            tallies_file.append(domain_dict[id]["tally"])
        tallies_file.export_to_xml()
        return domain_dict

            
class scale_xs:
    """
        Python class that reads in a scale t16 file and organizes the cross
        section data into a numpy dictionary. Currently set up to read an
        arbitrary number of energy groups, an arbitrary number of delayed
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
            'chi': self.t16_line([1, 1, 1, 2], False,
                                 ['CHI_T', 'CHI_P', 'CHI_D', 'DIFFCOEF']
                                 ),
            'detector': self.t16_line([4], False, ['RECIPVEL']),
            'Scattering cross': self.t16_line([], True, ['GTRANSFXS'])
        }
        XS_entries = ['REMXS', 'FISSXS', 'NSF', 'FISSE', 'DIFFCOEF',
                      'RECIPVEL', 'CHI_T', 'BETA_EFF', 'DECAY_CONSTANT',
                      'CHI_P', 'CHI_D', 'GTRANSFXS'
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
                        start = ii * self.num_groups
                        stop = start + self.num_groups
                        self.xs_lib[i][j][k]["REMXS"][ii] += np.sum(
                            self.xs_lib[i][j][k]["GTRANSFXS"][start:stop])\
                            - self.xs_lib[i][j][k]["GTRANSFXS"][start + ii]
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
                val = int(self.lines[k + 1].split()[0])
                if val not in uni:
                    uni.extend([val])
                m = uni.index(val)
                self.xs_lib[L][m][n]['BETA_EFF']\
                    .extend(beta_temp)
                self.xs_lib[L][m][n]['DECAY_CONSTANT']\
                    .extend(lam_temp)
            if 'branch no.' in line:
                index = line.find(',')
                L = int(line[index - 4:index])
                n = int(line.split()[-1])
            for key in self.catch.keys():
                if key in line:
                    if self.catch[key].multi_line_flag:
                        if (key == 'Betas'):
                            beta_temp = self.get_multi_line_values(k)
                        elif (key == 'Lambdas'):
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
        val = list(np.array(self.lines[k + 1].split()).astype(float))
        return val[index]

    def get_multi_line_values(self, k):
        values = []
        while True:
            val = self.lines[k + 1].split()
            k += 1
            for ent in val:
                try:
                    values.append(float(ent))
                except ValueError:
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
        num_temps = int(len(data['GC_UNIVERSE_NAME']) / (num_uni * num_burn))
        self.xs_lib = {}
        for i in range(num_burn):
            self.xs_lib[i] = {}
            for j in range(num_uni):
                self.xs_lib[i][j] = {}
                for k in range(num_temps):
                    self.xs_lib[i][j][k] = {}
                    index = i * (num_uni) + k * (num_burn * num_uni) + j
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
                    self.xs_lib[i][j][k]["CHI_T"] = list(
                        data['INF_CHIT'][index][::2])
                    self.xs_lib[i][j][k]["CHI_P"] = list(
                        data['INF_CHIP'][index][::2])
                    self.xs_lib[i][j][k]["CHI_D"] = list(
                        data['INF_CHID'][index][::2])
                    self.xs_lib[i][j][k]["BETA_EFF"] = list(
                        data['BETA_EFF'][index][2::2])
                    self.xs_lib[i][j][k]["DECAY_CONSTANT"] = list(
                        data['LAMBDA'][index][2::2])
                    self.xs_lib[i][j][k]["GTRANSFXS"] = list(
                        data['INF_SP0'][index][::2])


def read_input(fin, files):
    with open(fin) as f:
        lines = f.readlines()
    k = 0
    for k, line in enumerate(lines):
        if '[TITLE]' in line:
            f = open(lines[k + 1].split()[0], 'w')
        if '[MAT]' in line:
            mat_dict = {}
            num_mats = int(lines[k + 1].split()[0])
            val = lines[k + 2].split()
            for i in range(num_mats):
                mat_dict[val[i]] = {'temps': [],
                                    'file': [],
                                    'uni': [],
                                    'burn': [],
                                    'bran': []
                                    }
        if '[BRANCH]' in line:
            tot_branch = int(lines[k + 1].split()[0])
            for i in range(tot_branch):
                val = lines[k + 2 + i].split()
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
    out_dict = {}
    for material in mat_dict:
        out_dict[material] = {"temp": mat_dict[material]["temps"]}
        for i, temp in enumerate(mat_dict[material]["temps"]):
            file_index = mat_dict[material]["file"][i] - 1
            burnup_index = mat_dict[material]["burn"][i] - 1
            uni_index = mat_dict[material]["uni"][i] - 1
            branch_index = mat_dict[material]["bran"][i] - 1
            out_dict[material][str(temp)] = \
                files[file_index].xs_lib[burnup_index][uni_index][branch_index]
    f.write(json.dumps(out_dict, sort_keys=True, indent=4))


if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        description="Extracts Serpent 2 or SCALE group constants and puts \
                    them in a JSON file suitable for Moltres.")
    parser.add_argument(
        "input_file",
        type=str,
        nargs=1,
        help="*_res.m or *.t16 XS file from Serpent 2 or SCALE, respectively",
    )
    args = parser.parse_args()
    # import relevant modules for each software
    with open(sys.argv[1]) as f:
        lines = f.readlines()
    for k, line in enumerate(lines):
        if "FILES" in line:
            num_files = int(lines[k + 1].split()[0])
            files = {}
            for i in range(num_files):
                inputs = lines[k + 2 + i].split()
                XS_in, XS_t = inputs[0], inputs[1]
                if "scale" in XS_t:
                    files[i] = scale_xs(XS_in)
                elif "serpent" in XS_t:
                    from pyne import serpent
                    files[i] = serpent_xs(XS_in)
                elif "openmc" in XS_t:
                    XS_ref, XS_sum = inputs[2], inputs[3]
                    import openmc
                    if float(openmc.__version__[2:]) < 13.2:
                        raise Exception("moltres_xs.py is compatible with " +
                                        "OpenMC v0.13.2 or later only.")
                    sys.path.append('./')
                    openmc_ref_modules = {}
                    openmc_ref_modules[i] = importlib.import_module(
                        XS_ref.replace(".py", "")
                    )
                    files[i] = openmc_xs(XS_in, i, XS_sum)
                else:
                    raise (
                        "XS data not understood\n \
                            Please use: scale or serpent, or openmc"
                    )
    read_input(sys.argv[1], files)

    print("Successfully made JSON property file.")
