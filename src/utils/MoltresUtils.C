#include "MoltresUtils.h"
#include "MooseError.h"

namespace MoltresUtils
{

/**
 * Returns possible Cartesian direction vector components under the level-symmetric quadrature for
 * a given order N.
 */
std::vector<Real>
points(unsigned int N)
{
  switch (N)
  {
    case 2:
      return std::vector<Real>{0.5773502691896257};
    case 4:
      return std::vector<Real>{0.3500211745815407,
                               0.8688903007222011};
    case 6:
      return std::vector<Real>{0.2666354015167047,
                               0.6815077265365469,
                               0.9261809355174891};
    case 8:
      return std::vector<Real>{0.2182178902359924,
                               0.5773502691896257,
                               0.7867957924694432,
                               0.9511897312113419};
    case 10:
      return std::vector<Real>{0.1893213264780105,
                               0.5088817555826189,
                               0.6943188875943843,
                               0.8397599622366847,
                               0.9634909811104685};
    case 12:
      return std::vector<Real>{0.1672324971414912,
                               0.4595505230549429,
                               0.6280180398523312,
                               0.7600175218505538,
                               0.8722652169264515,
                               0.9716308886607312};
    case 14:
      return std::vector<Real>{0.1519858614610319,
                               0.4221569823047970,
                               0.5773502691896257,
                               0.6988920867759013,
                               0.8022262552314120,
                               0.8936910988743567,
                               0.9766271529257704};
    case 16:
      return std::vector<Real>{0.1389756946126266,
                               0.3922930709799660,
                               0.5370972569141674,
                               0.6504253018068668,
                               0.7467480720272295,
                               0.8319929644667765,
                               0.9092809811978063,
                               0.9804955444130666};
    case 18:
      return std::vector<Real>{0.1293481120858299,
                               0.3680446084547432,
                               0.5041653831086007,
                               0.6106624544193616,
                               0.7011665515053579,
                               0.7812556768832803,
                               0.8538655235895108,
                               0.9207671975517081,
                               0.9831267119754519};
    default:
      mooseError("Only even orders of N and N<20 are allowed.");
  }
}

/**
 * Returns possible weight values under the level-symmetric quadrature for a given order N.
 */
std::vector<Real>
weights(unsigned int N)
{
  switch (N)
  {
    case 2:
      return std::vector<Real>{1.};
    case 4:
      return std::vector<Real>{0.33333333333333333};
    case 6:
      return std::vector<Real>{0.17612613086338347,
                               0.15720720246994987};
    case 8:
      return std::vector<Real>{0.12098765432098764,
                               0.09074074074074068,
                               0.09259259259259299};
    case 10:
      return std::vector<Real>{0.08930314798435690,
                               0.07252915171236500,
                               0.04504376743641028,
                               0.05392811448783617};
    case 12:
      return std::vector<Real>{0.07077307572802660,
                               0.05587899520720631,
                               0.03734279426952707,
                               0.05025193141404591,
                               0.02586474723779406};
    case 14:
      return std::vector<Real>{0.05799704089700514,
                               0.04890079763678386,
                               0.02279353424122489,
                               0.03941320059498668,
                               0.03809908614408365,
                               0.02583940764180201,
                               0.00826957997290902};
    case 16:
      return std::vector<Real>{0.04899634978321998,
                               0.04132620269749813,
                               0.02031234525671927,
                               0.02654915237867853,
                               0.03787631708434387,
                               0.01355542870643232,
                               0.03259039327572704,
                               0.01038401511138594};
    case 18:
      return std::vector<Real>{0.04226679857587908,
                               0.03760984374258203,
                               0.01227772553689008,
                               0.03240535837316794,
                               0.00666131638823868,
                               0.03120767364508285,
                               0.01601092912564359,
                               0.02004736607851901,
                               0.00012094114211235,
                               0.01637415786841503};
    default:
      mooseError("Only even orders of N and N<20 are allowed.");
  }
}

/**
 * Returns all possible signed permutations of mu1,mu2,mu3.
 */
RealEigenMatrix
permute_octant(RealEigenMatrix m, unsigned int idx, Real mu1, Real mu2, Real mu3, Real w)
{
  RealEigenMatrix permutations(8, 4);
  permutations <<   mu1,  mu2,  mu3, w,
                   -mu1,  mu2,  mu3, w,
                    mu1, -mu2,  mu3, w,
                    mu1,  mu2, -mu3, w,
                   -mu1, -mu2,  mu3, w,
                   -mu1,  mu2, -mu3, w,
                    mu1, -mu2, -mu3, w,
                   -mu1, -mu2, -mu3, w;
  m.block(idx, 0, 8, 4) = permutations;
  return m;
}

/**
 * Returns a (N*(N+2))-by-4 RealEigenMatrix with each row consisting of the three components
 * of the Cartesian direction vector and its associated level-symmetric weight.
 */
RealEigenMatrix
level_symmetric(unsigned int N)
{
  RealEigenMatrix mu_w(N*(N+2),4);
  std::vector<Real> mu = points(N);
  std::vector<Real> weight = weights(N);
  unsigned int idx = 0;
  unsigned int w_idx = 0;
  // Cycles through possible values for the x & y components of the direction vector.
  // z component is fixed for given choices of x & y.
  for (unsigned int i = 0; i < (N+5)/6; ++i)
    for (unsigned int j = i; j < ((N/2-i)+1)/2; ++j)
    {
      unsigned int k = N / 2 - 1 - i - j;
      Real w = weight[w_idx];
      // For each unique combination of x,y,z component magnitudes, permute through all possible
      // permutations (including for positive and negative components).
      mu_w = permute_octant(mu_w, idx, mu[i], mu[j], mu[k], w);
      idx += 8;
      if (i == j && i == k)
      {
        // No extra permutations. Do nothing
      }
      else if (i == j)
      {
        mu_w = permute_octant(mu_w, idx, mu[k], mu[i], mu[i], w);
        idx += 8;
        mu_w = permute_octant(mu_w, idx, mu[i], mu[k], mu[i], w);
        idx += 8;
      }
      else if (j == k)
      {
        mu_w = permute_octant(mu_w, idx, mu[j], mu[i], mu[j], w);
        idx += 8;
        mu_w = permute_octant(mu_w, idx, mu[j], mu[j], mu[i], w);
        idx += 8;
      }
      else
      {
        mu_w = permute_octant(mu_w, idx, mu[k], mu[i], mu[j], w);
        idx += 8;
        mu_w = permute_octant(mu_w, idx, mu[j], mu[k], mu[i], w);
        idx += 8;
        mu_w = permute_octant(mu_w, idx, mu[k], mu[j], mu[i], w);
        idx += 8;
        mu_w = permute_octant(mu_w, idx, mu[i], mu[k], mu[j], w);
        idx += 8;
        mu_w = permute_octant(mu_w, idx, mu[j], mu[i], mu[k], w);
        idx += 8;
      }
      w_idx += 1;
    }
  return mu_w;
}

/**
 * Cartesian reflected flux ID maps. Refer to permute_octant for the permutation order of all flux
 * variables with the same direction vector components.
 */
unsigned int
x_reflection_map(unsigned int i)
{
  std::vector<unsigned int> vec{1, 0, 4, 5, 2, 3, 7, 6};
  return i - i % 8 + vec[i % 8];
}

unsigned int
y_reflection_map(unsigned int i)
{
  std::vector<unsigned int> vec{2, 4, 0, 6, 1, 7, 3, 5};
  return i - i % 8 + vec[i % 8];
}

unsigned int
z_reflection_map(unsigned int i)
{
  std::vector<unsigned int> vec{3, 5, 6, 0, 7, 1, 2, 4};
  return i - i % 8 + vec[i % 8];
}

/**
 * Returns the output of the real spherical harmonic function of degree l and order m, given an
 * input Cartesian direction vector (mu, eta, xi).
 */
Real
sph_harmonics(int l, int m, Real mu, Real eta, Real xi)
{
  const Real sqrt2 = 1.4142135623730951;
  const int abs_m = std::abs(m);
  const Real C = std::sqrt(factorial(l - abs_m) / factorial(l + abs_m));

  switch (l)
  {
    case 0:
      return 1.;
    case 1:
      switch (m)
      {
        case 1:
          return -sqrt2 * C * eta;
        case 0:
          return C * mu;
        case -1:
          return -sqrt2 * C * xi;
        default:
          mooseError("Invalid m value.");
      }
    case 2:
      switch (m)
      {
        case 2:
          return sqrt2 * C * 3 * (eta * eta - xi * xi);
        case 1:
          return -sqrt2 * C * 3 * mu * eta;
        case 0:
          return C * .5 * (3 * mu * mu - 1);
        case -1:
          return -sqrt2 * C * 3 * mu * xi;
        case -2:
          return sqrt2 * C * 6 * eta * xi;
        default:
          mooseError("Invalid m value.");
      }
    default:
      mooseError("Invalid l value.");
  }
}

int
factorial(int n)
{
  if (n == 0 || n == 1)
      return 1;
  return n * factorial(n - 1);
}

} // namespace MoltresUtils
