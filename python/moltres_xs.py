#!/usr/bin/env python3
# This script extracts homogenized group constants from Serpent 2 or SCALE into
# a JSON data file to be used with MoltresJsonMaterial.
import json
import sys
import argparse
import numpy as np
import importlib
import os

class openmc_mgxslib:
    """
        Reads OpenMC Statepoint and Summary Files, and mgxs.Library() to build and organize
        a dictionary that contains each MGXS Tally. Currently set up to read an arbitrary number of
        energy groups, delayed groups, temperatures, and materials.

        Parameters
        ----------
        stpt_file: str
            Name of OpenMC Statepoint HDF5 file containing collapsed cross section
            data
        mgxslib: openmc.mgxs.Library()
            MGXS Library that contains the multigroup tallies OpenMC processed through tallies.xml
        summ_file: str
            Name of OpenMC Summary file associated with the Statepoint file
        cleanup_h5: bool
            Deletes temporary .h5 files created during the process_mgxslib() function,
            may be turned off for logging/debugging purposes. Defaults to True.
        Returns
        ----------
        xs_lib: dict
            A hierarchical dictionary, organized by burnup, id, and temperature.
            Currently stores BETA_EFF, CHI_T, CHI_P, CHI_D, DECAY_CONSTANT, DIFFCOEF,
            FISSE, GTRANSFXS, NSF, RECIPVEL, FISSXS, TOTXS, and REMXS.

    """

    def __init__(self, stpt_file, mgxslib, summ_file, cleanup_h5: bool = True):
        import openmc
        import warnings
        version = float(openmc.__version__[2:])
        if version < 13.2:    # Needs testing to preview where it is compatible after
            raise Exception("moltres_xs.py is compatible with OpenMC " +
                            "version 0.13.2 or later only.")
        elif version > 15.3:
            warnings.warn("moltres_xs.py has not been tested for OpenMC " +
                          "versions newer than 0.14.0.")
        import inspect
        if isinstance(mgxslib, type(sys)): # Ensure to grab openmc.mgxs.Library from the python input file
            found = False
            for name, obj in inspect.getmembers(mgxslib):
                if isinstance(obj, openmc.mgxs.Library):
                    mgxslib = obj
                    found = True
                    break
            if not found:
                raise ValueError(f"No mgxs.Library object found in module {mgxslib}")

        cases = {
            "case": {
                "statepoint": stpt_file,
                "summary": summ_file,
                "mgxslib": mgxslib
            }
        }
        self.cases = cases
        self.clean = cleanup_h5
        self.json_store = {}
        self.xs_lib = {}
        self.required_mgxs_types = [
            "beta",
            "chi",
            "chi-prompt",
            "chi-delayed",
            "decay-rate",
            "diffusion-coefficient",
            "kappa-fission",
            "consistent nu-scatter matrix",
            "nu-fission",
            "inverse-velocity",
            "fission",
            "total"]

    def process_mgxslib(self, stpt_file, summ_file, mgxslib):
        import openmc

        statepoint = openmc.StatePoint(stpt_file, autolink = False)

        summary = openmc.Summary(summ_file)
        statepoint.link_with_summary(summary)
        if not hasattr(mgxslib, "load_from_statepoint"):
            raise TypeError("Passed Library Object invalid, must be mgxs.Library()")

        try:
            mgxslib.load_from_statepoint(statepoint)
        except Exception as e:
            print(f"Error loading statepoint: {e}")
            print("Ensure the statepoint file was generated using the same mgxs.Library definition.")
            raise e

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
            "inverse-velocity": "RECIPVEL"
        }

        mgxslib.build_hdf5_store()
        import h5py
        with h5py.File("mgxs/mgxs.h5", "r") as f:
            for domain_type in f:
                for domain_id in f[domain_type]:

                    mat_id = int(domain_id)
                    mat_name = material_id_to_name[mat_id]
                    mat = material_id_to_object[mat_id]

                    if mat_name not in self.json_store:
                        self.json_store[mat_name] = {}

                    if isinstance(mat.temperature, list):
                        temps = mat.temperature
                    else:
                        temps = [mat.temperature]

                    # Sort temperatures to map them to branch indices deterministically
                    # Assuming temps are numbers, or convertible to float. None is treated as 294K conceptually or handled separately
                    temps_sorted = sorted(temps, key=lambda x: float(x) if x is not None else 294.0)

                    for temp_idx, temp in enumerate(temps_sorted):
                        cache = {}
                        if temp is None:
                            raise ValueError(f"Material {mat_name} has no set temperature value. If you intended on using room temperature please set material.temperature = 294")
                        else:
                            temp = float(temp)

                        if str(temp) not in self.json_store[mat_name]:
                            self.json_store[mat_name][str(temp)] = {}

                        # Populate xs_lib for compatibility with read_input
                        burn_idx = 0
                        uni_idx = mat_id - 1
                        branch_idx = temp_idx

                        if burn_idx not in self.xs_lib:
                            self.xs_lib[burn_idx] = {}
                        if uni_idx not in self.xs_lib[burn_idx]:
                            self.xs_lib[burn_idx][uni_idx] = {}
                        if branch_idx not in self.xs_lib[burn_idx][uni_idx]:
                            self.xs_lib[burn_idx][uni_idx][branch_idx] = {}

                        for mgxs_type in self.required_mgxs_types:

                            if mgxs_type not in f[domain_type][domain_id]:
                                raise KeyError(f"MGXS type '{mgxs_type}' not found for material '{mat_name}'")

                            arr = f[domain_type][domain_id][mgxs_type]["average"][:]

                            if mgxs_type == "kappa-fission":
                                fission_data = np.array(f[domain_type][domain_id]["fission"]["average"][:])
                                kappa_data = np.array(arr)

                                arr = np.divide(kappa_data, fission_data,
                                                out = np.zeros_like(kappa_data, dtype = float),
                                                where = (fission_data != 0)) * 1e-6

                            if mgxs_type in ("consistent nu-scatter matrix", "total"):
                                if mgxs_type == "consistent nu-scatter matrix":
                                    raw_nuscatter = np.array(arr, copy = True)
                                    if arr.ndim == 3:
                                        arr = arr[:,:,0]
                                    else:
                                        print(f"Legendre Order of 0 Detected, Only P0 scattering is available for material: {mat_name} , results may differ from higher-order consistent scattering.")
                                cache[mgxs_type] = arr
                                cache["SPN"] = raw_nuscatter

                            if mgxs_type == "chi-delayed":
                                arr = arr.sum(axis = 0)
                                denom = arr.sum()

                                if denom!= 0:
                                    arr = arr / denom
                                else:
                                    arr = np.zeros_like(arr)

                            if mgxs_type == "beta":
                                arr = arr.sum(axis = 1)

                            if arr.ndim > 1:
                                arr = arr.flatten()

                            mgxs_key = rename_mgxs.get(mgxs_type, mgxs_type)

                            if mgxs_type == "consistent nu-scatter matrix":
                                mgxs_key = "GTRANSFXS"
                            elif mgxs_type == "kappa-fission":
                                mgxs_key = "FISSE"
                            elif mgxs_type == "nu-fission":
                                mgxs_key = "NSF" # New OpenMC nu-fission tally already comes flux weighted.
                            elif mgxs_type == "total":
                                mgxs_key = "TOTXS"

                            self.json_store[mat_name][str(temp)][mgxs_key] = arr.tolist()
                            self.xs_lib[burn_idx][uni_idx][branch_idx][mgxs_key] = arr.tolist()

                        existing_temps = set(self.json_store[mat_name].get("temp", [])) # Added this so advanced users get the same temp structure if doing multi-file
                        existing_temps.add(temp)
                        self.json_store[mat_name]["temp"] = sorted(existing_temps)

                        if "total" in cache and "consistent nu-scatter matrix" in cache:
                          total = cache["total"]
                          nuscatter = cache["consistent nu-scatter matrix"]
                          nuscatter = nuscatter.sum(axis = 1)

                          remxs = total - nuscatter
                          self.json_store[mat_name][str(temp)]["REMXS"] = remxs.tolist()
                          self.xs_lib[burn_idx][uni_idx][branch_idx]["REMXS"] = remxs.tolist()

                        if "SPN" in cache:
                          legendre_order = mgxslib.legendre_order
                          SPN = cache["SPN"]
                          if legendre_order > 0:
                            num_energy_groups = len(mgxslib.energy_groups.group_edges) - 1
                            num_pn = legendre_order + 1

                            SPN.shape = (num_energy_groups**2, num_pn)
                            SPN = SPN.T
                            self.json_store[mat_name][str(temp)]["SPN"] = SPN.tolist()
                            self.xs_lib[burn_idx][uni_idx][branch_idx]["SPN"] = SPN.tolist()
                        else:
                            SPN = SPN[:,:,0]
                            self.json_store[mat_name][str(temp)]["SPN"] = SPN.tolist()
                            self.xs_lib[burn_idx][uni_idx][branch_idx]["SPN"] = SPN.tolist()
                            print("SPN Calculation will only reflect P0 Scattering due to Legendre Order = 0")

                    print(f"Registered Moltres Group Constants for {mat_name} at {temp}K")
        if self.clean:
            try:
                os.remove("mgxs/mgxs.h5")
                os.rmdir("mgxs")
            except FileNotFoundError:
                pass

    def dump_json(self, json_path: str):
        """
        This function is for advanced users who wish to bypass the input file.
        Dumps the self.json_store dictionary into a JSON File.
        Organization that matches reference JSON files is not 100% guaranteed.

        Parameters
        ----------
        json_path: str
            Output path for the JSON File.
        Returns
        ----------
        JSON File.
        """
        if not self.json_store:
            raise ValueError("JSON Store is empty")

        with open(json_path, 'w') as f:
            json.dump(self.json_store , f, indent=4)
            print(f"Successfully Built and Dumped JSON at {json_path}")

    def build_json(self):
        """
        Runs process_mgxslib() for each case in self.cases
        self.cases is governed by the input file, which allows for multi-file processing.
        Works for 1 material, multiple temperatures. Multiple materials at 1 temperature.
        And multiple materials at multiple temperatures.

        Returns
        ---------
        json_store: dict
            Dictionary containing processed XS Data, this is used to also construct xs_lib.
        """
        for case in self.cases.values():
            self.process_mgxslib(
                case["statepoint"],
                case["summary"],
                case["mgxslib"]
            )
        return self.json_store

    def append_to_json(self, existing_json_path):
        """
        Function for advanced users who wish to append to a previously created JSON File.
        Works for adding new materials, temperatures, or rewriting old data.
        NOTE: Has NOT been tested with JSON Files that were created via the parser, only user created.

        Parameters
        ----------
        existing_json_path: str
            Path to the existing JSON File.
        """

        if not self.json_store:
            raise ValueError("Current JSON store is empty. Run build_json() first.")

        with open(existing_json_path, "r") as f:
            existing = json.load(f)

        for mat, mat_data in self.json_store.items():

            if mat not in existing:
                existing[mat] = mat_data
                continue

            for key, value in mat_data.items():

                if key == "temps":
                    old = set(existing[mat].get("temp", []))
                    new = set(value)
                    existing[mat]["temp"] = sorted(old.union(new))

                elif isinstance(value, dict):
                    if key not in existing[mat]:
                        existing[mat][key] = value
                    else:
                        existing[mat][key].update(value)
                else:
                    existing[mat][key] = value

        for mat in existing:
            if "temp" in existing[mat]:
                temps = existing[mat].pop("temp")
                existing[mat]["temp"] = temps

        with open(existing_json_path, "w") as f:
            json.dump(existing, f, indent=4)
            print(f"Successfully appended to JSON at {existing_json_path}")

    @staticmethod
    def generate_openmc_tallies_xml(energy_groups, delayed_groups: int, domains, geometry, tallies_file):
        """
            Generates a preloaded and configured mgxs.Library() to use with Moltres Multigroup Constants.
            Also exports those tallies to tallies.xml for use with OpenMC Runs. This function must be ran
            to use the input parser and recieve your JSON File.

            Parameters
            ----------
            energy_groups: list
                Energy groups to be used with OpenMC MGXS Tally creation, must include group edges.
            delayed_groups: int
                Delayed groups to be used with OpenMC MGXS Tally creation, must be an integer.
                NOTE: May add support for lists for backwards compatibility at a later date.
            domains: list
                Domains that MGXS tallies will be created over. All domains must be the same type,
                either openmc.Material or openmc.Cell. For singular domain entries please still use a list.
                Example: [my_material]
            geometry: openmc.Geometry()
                OpenMC Geometry that will be used to gather tallies and construct the mgxs.Library()
                This is required to construct the library.
            tallies_file: openmc.Tallies()
                Tallies object that OpenMC uses for storing tallies. Required to inject tallies from
                the mgxs.Library()

            Returns
            ---------
            mgxs_library: openmc.mgxs.Library()
                Library object that will be used to set up, construct, and record tallies.
        """
        import openmc
        import warnings
        version = float(openmc.__version__[2:])
        if version < 13.2:
            raise Exception("moltres_xs.py is compatible with OpenMC " +
                            "version 0.13.2 or later only.")
        elif version > 15.3:
            warnings.warn("moltres_xs.py has not been tested for OpenMC " +
                          "versions newer than 0.15.3.")

        if all(isinstance(d, openmc.Material) for d in domains):
            domain_type = "material"
        elif all(isinstance(d, openmc.Cell) for d in domains):
            domain_type = "cell"
        else:
            raise TypeError("All domains must be the same type (Material or Cell)")


        groups = openmc.mgxs.EnergyGroups(group_edges = energy_groups)
        mgxs_library = openmc.mgxs.Library(geometry)
        mgxs_library.energy_groups = groups
        mgxs_library.num_delayed_groups = delayed_groups
        mgxs_library.legendre_order = 3
        mgxs_library.domain_type = domain_type
        mgxs_library.domains = domains
        mgxs_library.mgxs_types = [
            "beta",
            "chi",
            "chi-prompt",
            "chi-delayed",
            "decay-rate",
            "diffusion-coefficient",
            "kappa-fission",
            "consistent nu-scatter matrix",
            "nu-fission",
            "inverse-velocity",
            "fission",
            "total"]

        mgxs_library.build_library()
        mgxs_library.add_to_tallies_file(tallies_file, merge = True) # Will change to add_to_tallies when OpenMC deprecates add_to_tallies_file (after 0.15.3)
        tallies_file.export_to_xml()

        return mgxs_library

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
                    files[i] = openmc_mgxslib(XS_in, openmc_ref_modules[i], XS_sum)
                    files[i].build_json()
                else:
                    raise ValueError(
                        "XS data not understood\n \
                            Please use: scale or serpent, or openmc"
                    )
    read_input(sys.argv[1], files)

    print("Successfully made JSON property file.")
