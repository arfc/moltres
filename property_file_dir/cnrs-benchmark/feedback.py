import numpy as np


def extrapolate(xs_name):
    """Extrapolate cross section based on thermal salt expansion feedback.

    Extrapolates cross section data at 900 K to 1500 K at 50 K intervals
    based on the thermal salt expansion feedback formula from [1]. Writes
    the extrapolated data back into the .txt cross section files in the
    Moltres-compatible format.
    
    Parameters
    ----------
    xs_name : list of str
        Names of cross sections to be extrapolated.

    Returns
    -------
    None

    References
    ----------
    [1] Tiberga et al., "Results from a multi-physics nurmerical benchmark for
    codes dedicated to molten salt fast reactors," Annals of Nuclear Energy,
    vol. 142, July 2020, 107428.
    """

    rho_900 = 2.0e3         # Density at 900 K [kg m-3]
    alpha = 2.0e-4          # Thermal expansion coeff [K-1]
    input_file = "benchmark_" + xs_name + ".txt"

    # Setup temperature values to extrapolate to
    temp = np.linspace(950, 1500, 12)

    # Read cross section data at 900K
    f = open(input_file, 'r+')
    lines = f.readlines()
    data_900 = list(lines[0].split())
    f.close()

    # Setup space separated data to be written back into txt
    s = " "
    xs = [s.join(data_900) + "\n"]
    h = open(input_file, 'w')

    for i in range(len(temp)):
        # Calculate density at temp[i]
        rho = rho_900 * (1 - alpha * (temp[i]-900))

        # Apply extrapolation formula at temp[i]
        data_next = [0, ] * len(data_900)
        data_next[0] = str(temp[i])
        for i in range(1, len(data_900)):
            if xs_name == "DIFFCOEF":
                data = float(data_900[i]) / rho * rho_900
                data_next[i] = '{:0.5e}'.format(data)
            else:
                data = float(data_900[i]) * rho / rho_900
                data_next[i] = '{:0.5e}'.format(data)

        data_next = s.join(data_next) + "\n"
        xs.append(data_next)

    # Write cross section data into txt file
    h = open(input_file, 'w')
    h.writelines(xs)
    h.close()
    return


def main():
    """Runs extrapolate() for the relevant cross sections.
    """
    xs_names = ["DIFFCOEF", "FISS", "NSF", "REMXS", "SP0"]
    for i in xs_names:
        extrapolate(i)
    return


if __name__ == "__main__":
    main()
