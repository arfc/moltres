#!/usr/bin/env python3
# This script extracts homogenized group constants from Serpent 2 or SCALE into
# a JSON data file to be used with MoltresJsonMaterial.
import json
import sys
import argparse
import numpy as np
import importlib


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
        Name of file containing collapsed cross section data
    file_num: int
        File number
    Returns
    ----------
    xs_lib: dict
        A hierarchical dictionary, organized by burnup, id, and temperature.
        Currently stores REMXS, FISSXS, NSF, FISSE, DIFFCOEF, RECIPVEL,
        CHI, BETA_EFF, DECAY_CONSTANT and GTRANSFXS.
    """

    def __init__(self, xs_filename, file_num, xs_summary):
        sp = openmc.StatePoint(xs_filename)
        summary = openmc.Summary(xs_summary)
        sp.link_with_summary(summary)
        domain_dict = openmc_ref_modules[file_num].domain_dict
        num_burn = 1
        num_uni = []
        for k in sp.filters:
            v = sp.filters[k]
            if isinstance(v, openmc.filter.MaterialFilter):
                num_uni.append(v.bins[0])
        self.xs_lib = {}
        for i in range(num_burn):
            self.xs_lib[i] = {}
            for j in num_uni:
                k = j - 1
                self.xs_lib[i][j - 1] = {}
                self.xs_lib[i][j - 1][k] = {}
                self.xs_lib[i][j - 1][k]["BETA_EFF"] = self.mgxs_tallies(
                    sp, domain_dict[j]["beta"]
                )
                self.xs_lib[i][j - 1][k]["CHI_T"] = self.mgxs_tallies(
                    sp, domain_dict[j]["chi"]
                )
                self.xs_lib[i][j - 1][k]["CHI_P"] = self.mgxs_tallies(
                    sp, domain_dict[j]["chiprompt"]
                )
                self.xs_lib[i][j - 1][k]["CHI_D"] = self.mgxs_tallies(
                    sp, domain_dict[j]["chidelayed"]
                )
                self.xs_lib[i][j - 1][k]["DECAY_CONSTANT"] = self.mgxs_tallies(
                    sp, domain_dict[j]["decayrate"]
                )
                self.xs_lib[i][j - 1][k]["DIFFCOEF"] = self.get_diffcoeff(
                    sp, domain_dict[j]["diffusioncoefficient"]
                )
                self.xs_lib[i][j - 1][k]["FISSE"] = self.get_fisse(
                    sp, domain_dict[j]["fissionxs"], domain_dict[j]["kappafissionxs"]
                )
                self.xs_lib[i][j - 1][k]["GTRANSFXS"] = self.get_scatter(
                    sp, domain_dict[j]["scatterprobmatrix"], domain_dict[j]["scatterxs"]
                )
                self.xs_lib[i][j - 1][k]["NSF"] = self.get_nsf(sp, j)
                self.xs_lib[i][j - 1][k]["RECIPVEL"] = self.mgxs_tallies(
                    sp, domain_dict[j]["inversevelocity"]
                )
                self.xs_lib[i][j - 1][k]["FISSXS"] = self.mgxs_tallies(
                    sp, domain_dict[j]["fissionxs"]
                )
                self.xs_lib[i][j - 1][k]["REMXS"] = self.get_remxs(
                    sp,
                    domain_dict[j]["scatterprobmatrix"],
                    domain_dict[j]["scatterxs"],
                    domain_dict[j]["absorptionxs"],
                )
        return

    def mgxs_tallies(self, sp, tally):
        tally.load_from_statepoint(sp)
        return list(tally.get_pandas_dataframe()["mean"])

    def get_diffcoeff(self, sp, tally):
        tally.load_from_statepoint(sp)
        df = tally.get_pandas_dataframe()
        df = df.loc[df["legendre"] == "P1"]
        return list(df["mean"])

    def get_fisse(self, sp, fissionxs, kappa):
        fissionxs.load_from_statepoint(sp)
        kappa.load_from_statepoint(sp)
        fissionxs_df = fissionxs.get_pandas_dataframe()
        kappa_df = kappa.get_pandas_dataframe()
        return list(kappa_df["mean"] / fissionxs_df["mean"] * 1e-6)

    def get_scatter(self, sp, prob_matrix, scatterxs):
        prob_matrix.load_from_statepoint(sp)
        scatterxs.load_from_statepoint(sp)
        scatterxs_df = scatterxs.get_pandas_dataframe()
        prob_matrix_df = prob_matrix.get_pandas_dataframe()
        group_nums = list(scatterxs_df["group in"])
        final_matrix_list = []
        for i in group_nums:
            scatter = float(scatterxs_df.loc[scatterxs_df["group in"] == i]["mean"])
            prob_list = np.array(
                prob_matrix_df.loc[prob_matrix_df["group in"] == i]["mean"]
            )
            final_matrix_list += list(prob_list * scatter)
        return final_matrix_list

    def get_nsf(self, sp, index):
        tally = sp.get_tally(name=str(index) + " tally")
        df = tally.get_pandas_dataframe()
        df_flux = np.array(df.loc[df["score"] == "flux"]["mean"])
        df_nu_fission = df.loc[df["score"] == "nu-fission"]
        nu_fission = list(np.array(df_nu_fission["mean"]) / df_flux)
        nu_fission.reverse()
        return nu_fission

    def get_remxs(self, sp, prob_matrix, scatterxs, absorbxs):
        prob_matrix.load_from_statepoint(sp)
        scatterxs.load_from_statepoint(sp)
        absorbxs.load_from_statepoint(sp)
        scatterxs_df = scatterxs.get_pandas_dataframe()
        prob_matrix_df = prob_matrix.get_pandas_dataframe()
        absorbxs_df = absorbxs.get_pandas_dataframe()
        group_nums = list(scatterxs_df["group in"])
        remxs = []
        for i in group_nums:
            scatter = float(scatterxs_df.loc[scatterxs_df["group in"] == i]["mean"])
            absxs = float(absorbxs_df.loc[absorbxs_df["group in"] == i]["mean"])
            out_scatter_prob = prob_matrix_df.loc[prob_matrix_df["group in"] == i]
            out_scatter_prob = np.array(
                out_scatter_prob.loc[out_scatter_prob["group out"] != i]["mean"]
            )
            remxs.append(sum(out_scatter_prob) * scatter + absxs)
        return remxs

    def generate_openmc_tallies_xml(
        energy_groups, delayed_groups, domains, domain_ids, tallies_file
    ):
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
            list of openmc domains
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
        import openmc.mgxs as mgxs

        groups = mgxs.EnergyGroups()
        groups.group_edges = np.array(energy_groups)
        big_group = mgxs.EnergyGroups()
        big_energy_group = [energy_groups[0], energy_groups[-1]]
        big_group.group_edges = np.array(big_energy_group)
        energy_filter = openmc.EnergyFilter(energy_groups)
        domain_dict = {}
        for id in domain_ids:
            domain_dict[id] = {}
        for domain, id in zip(domains, domain_ids):
            domain_dict[id]["beta"] = mgxs.Beta(
                domain=domain,
                energy_groups=big_group,
                delayed_groups=delayed_groups,
                name=str(id) + "_beta",
            )
            domain_dict[id]["chi"] = mgxs.Chi(
                domain=domain, groups=groups, name=str(id) + "_chi"
            )
            domain_dict[id]["chiprompt"] = mgxs.Chi(
                domain=domain, groups=groups, name=str(id) + "_chiprompt", prompt=True
            )
            domain_dict[id]["chidelayed"] = mgxs.ChiDelayed(
                domain=domain, energy_groups=groups, name=str(id) + "_chidelayed"
            )
            domain_dict[id]["decayrate"] = mgxs.DecayRate(
                domain=domain,
                energy_groups=big_group,
                delayed_groups=delayed_groups,
                name=str(id) + "_decayrate",
            )
            domain_dict[id]["diffusioncoefficient"] = mgxs.DiffusionCoefficient(
                domain=domain,
                groups=groups,
                name=str(id) + "_diffusioncoefficient",
            )
            domain_dict[id]["scatterprobmatrix"] = mgxs.ScatterProbabilityMatrix(
                domain=domain, groups=groups, name=str(id) + "_scatterprobmatrix"
            )
            domain_dict[id]["scatterxs"] = mgxs.ScatterXS(
                domain=domain, groups=groups, name=str(id) + "_scatterxs", nu=True
            )
            domain_dict[id]["inversevelocity"] = mgxs.InverseVelocity(
                domain=domain, groups=groups, name=str(id) + "_inversevelocity"
            )
            domain_dict[id]["fissionxs"] = mgxs.FissionXS(
                domain=domain, groups=groups, name=str(id) + "_fissionxs"
            )
            domain_dict[id]["kappafissionxs"] = mgxs.KappaFissionXS(
                domain=domain, groups=groups, name=str(id) + "_kappafissionxs"
            )
            domain_dict[id]["absorptionxs"] = mgxs.AbsorptionXS(
                domain=domain, groups=groups, name=str(id) + "_absorptionxs"
            )
            domain_dict[id]["tally"] = openmc.Tally(name=str(id) + " tally")
            domain_dict[id]["filter"] = openmc.MaterialFilter(domain)
            domain_dict[id]["tally"].filters = [
                domain_dict[id]["filter"],
                energy_filter,
            ]
            domain_dict[id]["tally"].scores = [
                "nu-fission",
                "flux",
            ]
            tallies_file += domain_dict[id]["beta"].tallies.values()
            tallies_file += domain_dict[id]["chi"].tallies.values()
            tallies_file += domain_dict[id]["chiprompt"].tallies.values()
            tallies_file += domain_dict[id]["chidelayed"].tallies.values()
            tallies_file += domain_dict[id]["decayrate"].tallies.values()
            tallies_file += domain_dict[id]["diffusioncoefficient"].tallies.values()
            tallies_file += domain_dict[id]["scatterprobmatrix"].tallies.values()
            tallies_file += domain_dict[id]["scatterxs"].tallies.values()
            tallies_file += domain_dict[id]["inversevelocity"].tallies.values()
            tallies_file += domain_dict[id]["fissionxs"].tallies.values()
            tallies_file += domain_dict[id]["kappafissionxs"].tallies.values()
            tallies_file += domain_dict[id]["absorptionxs"].tallies.values()
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
    -------
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
            "Betas": self.t16_line([], True, ["BETA_EFF"]),
            "Lambdas": self.t16_line([], True, ["DECAY_CONSTANT"]),
            "total-transfer": self.t16_line([1], False, ["REMXS"]),
            "fission": self.t16_line([0, 3, 4], False, ["FISSXS", "NSF", "FISSE"]),
            "chi": self.t16_line(
                [1, 1, 1, 2], False, ["CHI_T", "CHI_P", "CHI_D", "DIFFCOEF"]
            ),
            "detector": self.t16_line([4], False, ["RECIPVEL"]),
            "Scattering cross": self.t16_line([], True, ["GTRANSFXS"]),
        }
        XS_entries = [
            "REMXS",
            "FISSXS",
            "NSF",
            "FISSE",
            "DIFFCOEF",
            "RECIPVEL",
            "CHI_T",
            "BETA_EFF",
            "DECAY_CONSTANT",
            "CHI_P",
            "CHI_D",
            "GTRANSFXS",
        ]
        struct = self.lines[3].split()[:4]
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
                        self.xs_lib[i][j][k]["REMXS"][ii] += (
                            np.sum(self.xs_lib[i][j][k]["GTRANSFXS"][start:stop])
                            - self.xs_lib[i][j][k]["GTRANSFXS"][start + ii]
                        )
                        if self.xs_lib[i][j][k]["FISSE"][ii] != 0:
                            self.xs_lib[i][j][k]["FISSE"][ii] = (
                                self.xs_lib[i][j][k]["FISSE"][ii]
                                / self.xs_lib[i][j][k]["FISSXS"][ii]
                            )

    def get_xs(self):
        uni = []
        L = 0
        m = 0
        n = 0
        lam_temp = 0
        beta_temp = 0
        for k, line in enumerate(self.lines):
            if "Identifier" in line:
                val = int(self.lines[k + 1].split()[0])
                if val not in uni:
                    uni.extend([val])
                m = uni.index(val)
                self.xs_lib[L][m][n]["BETA_EFF"].extend(beta_temp)
                self.xs_lib[L][m][n]["DECAY_CONSTANT"].extend(lam_temp)
            if "branch no." in line:
                index = line.find(",")
                L = int(line[index - 4 : index])
                n = int(line.split()[-1])
            for key in self.catch.keys():
                if key in line:
                    if self.catch[key].multi_line_flag:
                        if key == "Betas":
                            beta_temp = self.get_multi_line_values(k)
                        elif key == "Lambdas":
                            lam_temp = self.get_multi_line_values(k)
                        else:
                            self.xs_lib[L][m][n][self.catch[key].xs_entry[0]].extend(
                                self.get_multi_line_values(k)
                            )
                    else:
                        for dex, xs in enumerate(self.catch[key].xs_entry):
                            dex = self.catch[key].index[dex]
                            self.xs_lib[L][m][n][xs].append(self.get_values(k, dex))

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
                except (ValueError):
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
            num_burn = len(np.unique(data["BURNUP"][:][0]))
        except (KeyError):
            num_burn = 1
        num_uni = len(np.unique(data["GC_UNIVERSE_NAME"]))
        num_temps = int(len(data["GC_UNIVERSE_NAME"]) / (num_uni * num_burn))
        self.xs_lib = {}
        for i in range(num_burn):
            self.xs_lib[i] = {}
            for j in range(num_uni):
                self.xs_lib[i][j] = {}
                for k in range(num_temps):
                    self.xs_lib[i][j][k] = {}
                    index = i * (num_uni) + k * (num_burn * num_uni) + j
                    self.xs_lib[i][j][k]["REMXS"] = list(data["INF_REMXS"][index][::2])
                    self.xs_lib[i][j][k]["FISSXS"] = list(data["INF_FISS"][index][::2])
                    self.xs_lib[i][j][k]["NSF"] = list(data["INF_NSF"][index][::2])
                    self.xs_lib[i][j][k]["FISSE"] = list(data["INF_KAPPA"][index][::2])
                    self.xs_lib[i][j][k]["DIFFCOEF"] = list(
                        data["INF_DIFFCOEF"][index][::2]
                    )
                    self.xs_lib[i][j][k]["RECIPVEL"] = list(
                        data["INF_INVV"][index][::2]
                    )
                    self.xs_lib[i][j][k]["CHI_T"] = list(data["INF_CHIT"][index][::2])
                    self.xs_lib[i][j][k]["CHI_P"] = list(data["INF_CHIP"][index][::2])
                    self.xs_lib[i][j][k]["CHI_D"] = list(data["INF_CHID"][index][::2])
                    self.xs_lib[i][j][k]["BETA_EFF"] = list(
                        data["BETA_EFF"][index][2::2]
                    )
                    self.xs_lib[i][j][k]["DECAY_CONSTANT"] = list(
                        data["LAMBDA"][index][2::2]
                    )
                    self.xs_lib[i][j][k]["GTRANSFXS"] = list(
                        data["INF_SP0"][index][::2]
                    )


def read_input(fin, files):
    with open(fin) as f:
        lines = f.readlines()
    k = 0
    for k, line in enumerate(lines):
        if "[TITLE]" in line:
            f = open(lines[k + 1].split()[0], "w")
        if "[MAT]" in line:
            mat_dict = {}
            num_mats = int(lines[k + 1].split()[0])
            val = lines[k + 2].split()
            for i in range(num_mats):
                mat_dict[val[i]] = {
                    "temps": [],
                    "file": [],
                    "uni": [],
                    "burn": [],
                    "bran": [],
                }
        if "[BRANCH]" in line:
            tot_branch = int(lines[k + 1].split()[0])
            for i in range(tot_branch):
                val = lines[k + 2 + i].split()
                mat_dict[val[0]]["temps"].extend([int(val[1])])
                mat_dict[val[0]]["file"].extend([int(val[2])])
                mat_dict[val[0]]["burn"].extend([int(val[3])])
                mat_dict[val[0]]["uni"].extend([int(val[4])])
                mat_dict[val[0]]["bran"].extend([int(val[5])])

    out_dict = {}
    for material in mat_dict:
        out_dict[material] = {"temp": mat_dict[material]["temps"]}
        for i, temp in enumerate(mat_dict[material]["temps"]):
            file_index = mat_dict[material]["file"][i] - 1
            burnup_index = mat_dict[material]["burn"][i] - 1
            uni_index = mat_dict[material]["uni"][i] - 1
            branch_index = mat_dict[material]["bran"][i] - 1
            out_dict[material][str(temp)] = files[file_index].xs_lib[burnup_index][
                uni_index
            ][branch_index]
    f.write(json.dumps(out_dict, sort_keys=True, indent=4))


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="Extracts Serpent 2 or SCALE or OpenMC group constants \
            and puts them in a JSON file suitable for Moltres."
    )
    parser.add_argument(
        "input_file",
        type=str,
        nargs=1,
        help="*_res.m or *.t16 XS or *.h5 \
                            file from Serpent 2 or SCALE or OpenMC, \
                            respectively",
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
                XS_in, XS_t, XS_ref, XS_sum = lines[k + 2 + i].split()
                if "scale" in XS_t:
                    files[i] = scale_xs(XS_in)
                elif "serpent" in XS_t:
                    from pyne import serpent

                    files[i] = serpent_xs(XS_in)
                elif "openmc" in XS_t:
                    import openmc
                    import openmc.mgxs as mgxs

                    openmc_ref_modules = {}
                    openmc_ref_modules[i] = importlib.import_module(
                        XS_ref.replace(".py", "")
                    )
                    files[i] = openmc_xs(XS_in, i, XS_sum)
                else:
                    raise (
                        "XS data not understood\n \
                            Please use: Scale or Serpent or OpenMC"
                    )
    read_input(sys.argv[1], files)

    print("Successfully made JSON property file.")
