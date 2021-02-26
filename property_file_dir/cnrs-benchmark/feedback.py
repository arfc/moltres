import numpy as np


def extrapolate(xs_name):
    """ Extrapolates cross section data from 900 K to 1500 K at 50 K intervals
    based on the formula from Tiberga et al. (2020)
    """

    rho_900 = 2.0e3         # Density at 900 K [kg m-3]
    alpha = 2.0e-4      # Thermal expansion coeff [K-1]
    input_file = "benchmark_" + xs_name + ".txt"

    # Setup temperature values
    temp = np.linspace(950, 1500, 12)

    f = open(input_file, 'r+')
    lines = f.readlines()
    data_900 = list(lines[0].split())
    f.close()

    s = " "
    xs = [s.join(data_900) + "\n"]
    h = open(input_file, 'w')

    for i in range(len(temp)):
        # Calculate density at 1500 K
        rho = rho_900 * (1 - alpha * (temp[i]-900))

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

    h = open(input_file, 'w')
    h.writelines(xs)
    h.close()
    return


def main():
    xs_names = ["DIFFCOEF", "FISS", "NSF", "REMXS", "SP0"]
    for i in xs_names:
        extrapolate(i)
    return


main()
