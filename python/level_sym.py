import numpy as np

# %% mu1 values

# Retrieved from Wang et al., 2018. Rattlesnake Theory Manual.
mu1 = {2: 0.5773502691896257,
       4: 0.3500211745815407,
       6: 0.2666354015167047,
       8: 0.2182178902359924,
       10: 0.1893213264780105,
       12: 0.1672324971414912,
       14: 0.1519858614610319,
       16: 0.1389756946126266,
       18: 0.1293481120858299}

# %% Functions to calculate mu and weight values


def get_mu(N, mu1):
    mu = np.empty(N//2)
    mu[0] = mu1
    # print(mu1)
    for i in range(1, N//2):
        mu[i] = np.sqrt(mu1 ** 2 + i * 2 * (1 - 3 * mu1 ** 2) / (N - 2))
        # print(mu[i])
    return mu


def get_weights(N, mu):
    moments = [[0, 0], [4, 0], [6, 0], [8, 0], [10, 0], [12, 0], [6, 6],
               [14, 0], [16, 0], [8, 8]]
    n = 1 + int(np.round(np.floor((N * (N + 8) - 1) / 48)))
    # n = int(round(N * (N + 2) / 8))
    idx = []
    for i in range(1, N//2+1):
        for j in range(i, N//2+1):
            eta = N//2 + 2 - i - j
            if j <= eta:
                idx.append([i, j, eta])

    all_idx = []
    for i in range(1, N//2+1):
        for j in range(1, N//2+1):
            eta = N//2 + 2 - i - j
            if eta > 0:
                all_idx.append([int(i), int(j), int(eta)])

    A = np.zeros((n, n))
    b = np.zeros(n)
    for i in range(n):
        for j in range(len(idx)):
            for k in range(len(all_idx)):
                if sorted(all_idx[k]) == sorted(idx[j]):
                    A[i, j] += \
                        mu[all_idx[k][0]-1] ** moments[i][0] * \
                        mu[all_idx[k][1]-1] ** moments[i][1]
        b[i] = 1 / (moments[i][0] + 1)
        if moments[i][1] > 0:
            for k in range(moments[i][1]):
                if k % 2 == 1:
                    b[i] *= k / (moments[i][0] + k + 2)

    return np.linalg.solve(A, b)


def print_vals():
    for i in np.arange(2, 20, 2):
        mu = get_mu(i, mu1[i])
        weights = get_weights(i, mu)
        print("For N = " + str(i) + ", the mu values are:")
        for j in mu:
            print(j)
        print("and the weight values are:")
        for k in weights:
            print(k)
        print("--------------------")


def index(N):
    total_N = (N * (N + 2))
    mu = get_mu(N, mu1[N])
    m = np.zeros((total_N, 3))
    w_map = np.zeros(total_N)
    idx = 0
    w_idx = 0
    for i in range((N+5)//6):
        for j in range(i, ((N//2-i)+1)//2):
            k = N // 2 - i - j - 1
            m[idx:idx+8] = perm_oct(mu[i], mu[j], mu[k])
            w_map[idx:idx+8] = [w_idx,] * 8
            idx += 8
            if (i == j and i == k):
                pass
            elif (i == j):
                m[idx:idx+8] = perm_oct(mu[k], mu[i], mu[i])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
                m[idx:idx+8] = perm_oct(mu[i], mu[k], mu[i])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
            elif (j == k):
                m[idx:idx+8] = perm_oct(mu[j], mu[i], mu[j])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
                m[idx:idx+8] = perm_oct(mu[j], mu[j], mu[i])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
            else:
                m[idx:idx+8] = perm_oct(mu[i], mu[k], mu[j])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
                m[idx:idx+8] = perm_oct(mu[j], mu[i], mu[k])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
                m[idx:idx+8] = perm_oct(mu[j], mu[k], mu[i])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
                m[idx:idx+8] = perm_oct(mu[k], mu[i], mu[j])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
                m[idx:idx+8] = perm_oct(mu[k], mu[j], mu[i])
                w_map[idx:idx+8] = [w_idx,] * 8
                idx += 8
            w_idx += 1
    return m, w_map


def perm_oct(i, j, k):
    m = np.zeros((8, 3))
    m[0] = i, j, k
    m[1] = -i, j, k
    m[2] = i, -j, k
    m[3] = i, j, -k
    m[4] = -i, -j, k
    m[5] = -i, j, -k
    m[6] = i, -j, -k
    m[7] = -i, -j, -k
    return m


# %% Print mu and weight values

print_vals()
