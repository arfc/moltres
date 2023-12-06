#include "SNScattering.h"

#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNScattering);

InputParameters
SNScattering::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups");
  params.addParam<int>("L", 2, "Maximum scattering moment");
  params.addRequiredCoupledVar("group_angular_fluxes",
      "All the variables that hold the group angular fluxes. These MUST be listed by decreasing "
      "energy/increasing group number.");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}

SNScattering::SNScattering(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _scatter(getMaterialProperty<std::vector<Real>>("scatter")),
    _d_scatter_d_temp(getMaterialProperty<std::vector<Real>>("d_scatter_d_temp")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
    _L(getParam<int>("L")),
    _temp_id(coupled("temperature"))
{
  unsigned int n = coupledComponents("group_angular_fluxes");
  if (n != _num_groups)
    mooseError("The number of coupled variables doesn't match the number of groups.");
  _group_fluxes.resize(n);
  _flux_ids.resize(n);
  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    _group_fluxes[g] = &coupledArrayValue("group_angular_fluxes", g);
    _flux_ids[g] = coupled("group_angular_fluxes", g);
  }

  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNScattering::computeQpResidual(RealEigenVector & residual)
{
  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);

  std::vector<std::vector<std::vector<Real>>>
    flux_moments(_num_groups, std::vector<std::vector<Real>>
        { {0.}, {0., 0., 0.}, {0., 0., 0., 0., 0.} });
  for (unsigned int g = 0; g < _num_groups; ++g)
    for (int l = 0; l < _L+1; ++l)
    {
      int scatter_idx = l * _num_groups * _num_groups + g * _num_groups + _group;
      if (_scatter[_qp][scatter_idx] == 0.)
        continue;
      for (int m = -l; m < l+1; ++m)
        for (unsigned int i = 0; i < _count; ++i)
        {
          flux_moments[g][l][m+l] +=
            _weights(i) * MoltresUtils::sph_harmonics(l, m, _ordinates(i,0), _ordinates(i,1),
            _ordinates(i,2)) * (*_group_fluxes[g])[_qp](i);
        }
    }

  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    for (int l = 0; l < _L+1; ++l)
    {
      int scatter_idx = l * _num_groups * _num_groups + g * _num_groups + _group;
      if (_scatter[_qp][scatter_idx] == 0.)
        continue;
      for (int m = -l; m < l+1; ++m)
        for (unsigned int i = 0; i < _count; ++i)
          residual(i) -= _scatter[_qp][scatter_idx] * flux_moments[g][l][m+l] *
            (2. * l + 1.) / 8. * MoltresUtils::sph_harmonics(l, m,
            _ordinates(i,0), _ordinates(i,1), _ordinates(i,2));
    }
  }
  residual = _weights.cwiseProduct(lhs).cwiseProduct(residual);
}

RealEigenVector
SNScattering::computeQpJacobian()
{
  RealEigenVector jac = RealEigenVector::Zero(_count);

  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);

  for (int l = 0; l < _L+1; ++l)
  {
    int scatter_idx = l * _num_groups * _num_groups + _group * _num_groups + _group;
    for (int m = -l; m < l+1; ++m)
      for (unsigned int i = 0; i < _count; ++i)
        jac(i) -= _scatter[_qp][scatter_idx] * Utility::pow<2>(_weights(i)) *
          (2. * l + 1.) / 8. * Utility::pow<2>(
          MoltresUtils::sph_harmonics(l, m, _ordinates(i,0), _ordinates(i,1),
          _ordinates(i,2))) * _phi[_j][_qp];
  }

  return _weights.cwiseProduct(lhs).cwiseProduct(jac);
}

RealEigenMatrix
SNScattering::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);

  if (jvar.number() == _temp_id)
  {
    RealEigenMatrix jac(_count, 1);

    std::vector<std::vector<std::vector<Real>>>
      flux_moments(_num_groups, std::vector<std::vector<Real>>
          { {0.}, {0., 0., 0.}, {0., 0., 0., 0., 0.} });
    for (unsigned int g = 0; g < _num_groups; ++g)
      for (int l = 0; l < _L+1; ++l)
        for (int m = -l ; m < l+1; ++m)
          for (unsigned int i = 0; i < _count; ++i)
          {
            flux_moments[g][l][m+l] +=
              _weights(i) * MoltresUtils::sph_harmonics(l, m, _ordinates(i,0), _ordinates(i,1),
              _ordinates(i,2)) * (*_group_fluxes[g])[_qp](i);
          }

    for (unsigned int g = 0; g < _num_groups; ++g)
    {
      for (int l = 0; l < _L+1; ++l)
      {
        int scatter_idx = l * _num_groups * _num_groups + g * _num_groups + _group;
        if (_scatter[_qp][scatter_idx] == 0.)
          continue;
        for (int m = -l; m < l+1; ++m)
          for (unsigned int i = 0; i < _count; ++i)
            jac(i) -= _weights(i) * lhs(i) * _d_scatter_d_temp[_qp][scatter_idx] *
              flux_moments[g][l][m+l] * (2. * l + 1.) / 8. *
              MoltresUtils::sph_harmonics(l, m, _ordinates(i,0), _ordinates(i,1), _ordinates(i,2))
              * _phi[_j][_qp];
      }
    }
    return jac;
  }
  for (unsigned int g = 0; g < _num_groups; ++g)
  {
    if (jvar.number() == _flux_ids[g])
    {
        RealEigenMatrix jac = RealEigenMatrix::Zero(_count, _count);

        for (int l = 0; l < _L+1; ++l)
        {
          int scatter_idx = l * _num_groups * _num_groups + g * _num_groups + _group;
          if (_scatter[_qp][scatter_idx] == 0.)
            continue;
          for (int m = -l; m < l+1; ++m)
            for (unsigned int i = 0; i < _count; ++i)
              for (unsigned int j = 0; j < _count; ++j)
                jac(i, j) -= _weights(i) * lhs(i) * _scatter[_qp][scatter_idx] * _weights(j) *
                  MoltresUtils::sph_harmonics(l, m, _ordinates(j,0), _ordinates(j,1),
                  _ordinates(j,2)) * _phi[_j][_qp] * (2. * l + 1.) / 8. *
                  MoltresUtils::sph_harmonics(l, m,
                  _ordinates(i,0), _ordinates(i,1), _ordinates(i,2));
        }
        return jac;
    }
  }
  return ArrayKernel::computeQpOffDiagJacobian(jvar);
}
