import sys


def calc_feedback_coeff(k1, k2, T1, T2):
    '''
    This function takes two multiplication factors and
    two temperatures and calculates the feedback coefficient
    '''
    return (1. / k1 - 1. / k2) / (T2 - T1) * 1e5


print(calc_feedback_coeff(*[float(el) for el in sys.argv[1:5]]))
