#include "SNTransport.h"
#include "MoltresUtils.h"

registerMooseObject("MoltresApp", SNTransport);

InputParameters
SNTransport::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups");
  params.addParam<bool>("has_scatter", true, "Whether scattering occurs in the system");
  params.addParam<int>("L", 2, "Maximum scattering moment");
  params.addRequiredCoupledVar("group_angular_fluxes",
      "All the variables that hold the group angular fluxes. These MUST be listed by decreasing "
      "energy/increasing group number.");
  params.addParam<std::vector<FunctionName>>("source_function",
                                             "Externel neutron source functions");
  params.addCoupledVar("temperature", "The temperature used to interpolate material properties");
  return params;
}

SNTransport::SNTransport(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _totxs(getMaterialProperty<std::vector<Real>>("totxs")),
    _d_totxs_d_temp(getMaterialProperty<std::vector<Real>>("d_totxs_d_temp")),
    _scatter(getMaterialProperty<std::vector<Real>>("scatter")),
    _d_scatter_d_temp(getMaterialProperty<std::vector<Real>>("d_scatter_d_temp")),
    _N(getParam<unsigned int>("N")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
    _has_scatter(getParam<bool>("has_scatter")),
    _has_ext_src(isParamValid("source_function")),
    _L(getParam<int>("L")),
    _temp_id(coupled("temperature"))
{
  if (_has_ext_src)
  {
    auto & funcs = getParam<std::vector<FunctionName>>("source_function");
    if (_count != funcs.size())
      paramError("source_function",
                 "Number of external neutron source functions must agree with the number of "
                 "angular flux array variable components.");
    for (auto & func : funcs)
      _func.push_back(&getFunctionByName(func));
  }

  if (_has_scatter)
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
  }

  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(_N);
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNTransport::computeQpResidual(RealEigenVector & residual)
{
  RealEigenVector ord_grad_test = _ordinates * _array_grad_test[_i][_qp];
  // LHS for the weak forms of source terms, including the weights
  RealEigenVector lhs = _weights.cwiseProduct(
      RealEigenVector::Constant(_count, _test[_i][_qp]) + _tau_sn[_qp][_group] * ord_grad_test);

  // Streaming
  residual = _weights.cwiseProduct(ord_grad_test).cwiseProduct(
      _tau_sn[_qp][_group] * _ordinates.cwiseProduct(_grad_u[_qp]).rowwise().sum() -
      ((1. - _tau_sn[_qp][_group] * _totxs[_qp][_group]) * _u[_qp]));

  // Collision
  residual += _test[_i][_qp] * _totxs[_qp][_group] * _weights.cwiseProduct(_u[_qp]);

  // Scattering
  if (_has_scatter)
  {
    RealEigenVector scatter_res = RealEigenVector::Zero(_count);
    std::vector<std::vector<std::vector<Real>>>
      flux_moments(_num_groups, std::vector<std::vector<Real>>{ {0}, {0, 0, 0}, {0, 0, 0, 0, 0} });
    for (unsigned int g = 0; g < _num_groups; ++g)
      for (int l = 0; l < _L+1; ++l)
      {
        int scatter_idx = l * _num_groups * _num_groups + g * _num_groups + _group;
        if (_scatter[_qp][scatter_idx] == 0.)
          continue;
        for (int m = -l ; m < l+1; ++m)
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
            scatter_res(i) += _scatter[_qp][scatter_idx] * flux_moments[g][l][m+l] *
              (2. * l + 1.) / 8. * MoltresUtils::sph_harmonics(l, m,
              _ordinates(i,0), _ordinates(i,1), _ordinates(i,2));
      }
    }
    residual -= lhs.cwiseProduct(scatter_res);
  }

  // External source
  if (_has_ext_src)
    for (unsigned int i = 0; i < _count; ++i)
      residual(i) -= lhs(i) * _func[i]->value(_t, _q_point[_qp]);
}

RealEigenVector
SNTransport::computeQpJacobian()
{
  RealEigenVector jac(_count);

  RealEigenVector ord_grad_test = _ordinates * _array_grad_test[_i][_qp];
  // LHS for the weak forms of source terms
  RealEigenVector lhs = _weights.cwiseProduct(
      RealEigenVector::Constant(_count, _test[_i][_qp]) + _tau_sn[_qp][_group] * ord_grad_test);
  RealEigenVector array_phi = RealEigenVector::Constant(_count, _phi[_j][_qp]);
  RealEigenVector array_grad_phi(3);
  array_grad_phi << _grad_phi[_j][_qp](0),
                    _grad_phi[_j][_qp](1),
                    _grad_phi[_j][_qp](2);

  // Streaming
  jac = _weights.cwiseProduct(ord_grad_test).cwiseProduct(
      _tau_sn[_qp][_group] * (_ordinates * array_grad_phi) -
      (1. - _tau_sn[_qp][_group] * _totxs[_qp][_group]) * array_phi);

  // Collision
  jac += _test[_i][_qp] * _totxs[_qp][_group] * _phi[_j][_qp] * _weights;

  if (_has_scatter)
  {
    RealEigenVector scatter_jac = RealEigenVector::Zero(_count);

    for (int l = 0; l < _L+1; ++l)
    {
      int scatter_idx = l * _num_groups * _num_groups + _group * _num_groups + _group;
      for (int m = -l; m < l+1; ++m)
        for (unsigned int i = 0; i < _count; ++i)
          scatter_jac(i) += _scatter[_qp][scatter_idx] * Utility::pow<2>(_weights(i)) *
            (2. * l + 1.) / 8. * Utility::pow<2>(
            MoltresUtils::sph_harmonics(l, m, _ordinates(i,0), _ordinates(i,1),
            _ordinates(i,2))) * _phi[_j][_qp];
    }
    jac -= lhs.cwiseProduct(scatter_jac);
  }

  return jac;
}

RealEigenMatrix
SNTransport::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  RealEigenVector ord_grad_test = _ordinates * _array_grad_test[_i][_qp];
  // LHS for the weak forms of source terms
  RealEigenVector lhs = _weights.cwiseProduct(
      RealEigenVector::Constant(_count, _test[_i][_qp]) + _tau_sn[_qp][_group] * ord_grad_test);

  if (jvar.number() == _temp_id)
  {
    RealEigenMatrix jac(_count, 1);

    // Streaming
    jac = _weights.cwiseProduct(ord_grad_test).cwiseProduct(
        _tau_sn[_qp][_group] * _d_totxs_d_temp[_qp][_group] * _phi[_j][_qp] * _u[_qp]);

    // Collision
    jac += _test[_i][_qp] * _d_totxs_d_temp[_qp][_group] * _phi[_j][_qp] *
      _weights.cwiseProduct(_u[_qp]);

    return jac;
  }
  if (_has_scatter)
  {
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
  }
  return ArrayKernel::computeQpOffDiagJacobian(jvar);
}
