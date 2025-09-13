#include "SNFission.h"
#include "Executioner.h"

registerMooseObject("MoltresApp", SNFission);

InputParameters
SNFission::validParams()
{
  InputParameters params = ArrayKernel::validParams();
  params.addRequiredParam<unsigned int>("N", "Discrete ordinate order");
  params.addRequiredParam<unsigned int>("group_number", "The current energy group");
  params.addRequiredParam<unsigned int>("num_groups", "The total number of energy groups");
  params.addCoupledVar("group_angular_fluxes",
      "All the variables that hold the group angular fluxes. These MUST be listed by decreasing "
      "energy/increasing group number.");
  params.addCoupledVar("group_fluxes",
      "All the variables that hold the group fluxes. These MUST be listed by decreasing "
      "energy/increasing group number.");
  params.addRequiredParam<bool>("account_delayed", "Whether to account for delayed neutrons.");
  params.addParam<PostprocessorName>(
      "eigenvalue_scaling", 1.0, "Artificial scaling factor for the fission source.");
  params.addParam<bool>("acceleration", false, "Whether an acceleration scheme is applied.");
  params.addParam<bool>("use_initial_flux", false,
      "Whether to use the diffusion flux as an initial condition");
  params.addParam<PostprocessorName>("iteration_postprocessor", 0,
      "Name of postprocessor with fixed point iteration number");
  return params;
}

SNFission::SNFission(const InputParameters & parameters)
  : ArrayKernel(parameters),
    _tau_sn(getMaterialProperty<std::vector<Real>>("tau_sn")),
    _nsf(getMaterialProperty<std::vector<Real>>("nsf")),
    _chi_t(getMaterialProperty<std::vector<Real>>("chi_t")),
    _chi_p(getMaterialProperty<std::vector<Real>>("chi_p")),
    _beta(getMaterialProperty<Real>("beta")),
    _group(getParam<unsigned int>("group_number") - 1),
    _num_groups(getParam<unsigned int>("num_groups")),
    _account_delayed(getParam<bool>("account_delayed")),
    _eigenvalue_scaling(getPostprocessorValue("eigenvalue_scaling")),
    _acceleration(getParam<bool>("acceleration")),
    _use_initial_flux(getParam<bool>("use_initial_flux")),
    _iteration_postprocessor(getPostprocessorValue("iteration_postprocessor"))
{
  if (_acceleration || _use_initial_flux)
  {
    unsigned int n = coupledComponents("group_fluxes");
    if (n != _num_groups)
      mooseError("The number of coupled variables does not match the number of groups.");
    _group_fluxes.resize(n);
    for (unsigned int g = 0; g < _num_groups; ++g)
      _group_fluxes[g] = &coupledValue("group_fluxes", g);
  }

  // When using the hybrid SN-diffusion method, not using the diffusion flux as the initial
  // condition allows the SN subsolver to converge faster
  if (!(_acceleration))
  {
    unsigned int n = coupledComponents("group_angular_fluxes");
    if (n != _num_groups)
      mooseError("The number of coupled variables doesn't match the number of groups.");
    _group_angular_fluxes.resize(n);
    _flux_ids.resize(n);
    for (unsigned int g = 0; g < _num_groups; ++g)
    {
      _group_angular_fluxes[g] = &coupledArrayValue("group_angular_fluxes", g);
      _flux_ids[g] = coupled("group_angular_fluxes", g);
    }
  }

  // Level-symmetric quadrature points and weights
  RealEigenMatrix ords_weights = MoltresUtils::level_symmetric(getParam<unsigned int>("N"));
  _ordinates = ords_weights.leftCols(3);
  _weights = ords_weights.col(3);
}

void
SNFission::computeQpResidual(RealEigenVector & residual)
{
  // Skip computations if fission source spectrum is zero
  if (_chi_p[_qp][_group] == 0.0 && _chi_t[_qp][_group] == 0.0)
    return;

  RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
    RealEigenVector::Constant(_count, _test[_i][_qp]);

  // If using diffusion flux as initial condition, no need to compute fission source using
  // angular fluxes on the first iteration.
  Real fission = 0.;
  if (_acceleration || (_use_initial_flux && _iteration_postprocessor == 1.0))
  {
    for (unsigned int g = 0; g < _num_groups; ++g)
      fission += _ls_norm_factor * _nsf[_qp][g] * (*_group_fluxes[g])[_qp];
  }
  else
  {
    for (unsigned int g = 0; g < _num_groups; ++g)
      fission += _ls_norm_factor * (
          _nsf[_qp][g] * _weights.transpose() * (*_group_angular_fluxes[g])[_qp])(0);
  }

  if (_account_delayed)
    fission *= (1. - _beta[_qp]) * _chi_p[_qp][_group];
  else
    fission *= _chi_t[_qp][_group];

  if (_eigenvalue_scaling != 1.0)
    fission /= _eigenvalue_scaling;

  residual = -_weights.cwiseProduct(lhs) * fission;
}

RealEigenVector
SNFission::computeQpJacobian()
{
  // Skip computations if fission source spectrum is zero
  if (_chi_p[_qp][_group] == 0.0 && _chi_t[_qp][_group] == 0.0)
    return ArrayKernel::computeQpJacobian();

  // If using diffusion flux as initial condition, no dependence on angular fluxes on the first
  // iteration.
  if (_acceleration || (_use_initial_flux && _iteration_postprocessor == 1.0))
    return ArrayKernel::computeQpJacobian();
  else
  {
    RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
      RealEigenVector::Constant(_count, _test[_i][_qp]);

    Real fission = -_ls_norm_factor * _nsf[_qp][_group] * _phi[_j][_qp];

    if (_account_delayed)
      fission *= (1. - _beta[_qp]) * _chi_p[_qp][_group];
    else
      fission *= _chi_t[_qp][_group];

    if (_eigenvalue_scaling != 1.0)
      fission /= _eigenvalue_scaling;

    return _weights.cwiseProduct(lhs).cwiseProduct(_weights * fission);
  }
}

RealEigenMatrix
SNFission::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
  // Skip computations if fission source spectrum is zero
  if (_chi_p[_qp][_group] == 0.0 && _chi_t[_qp][_group] == 0.0)
    return ArrayKernel::computeQpOffDiagJacobian(jvar);

  // If using diffusion flux as initial condition, no dependence on angular fluxes on the first
  // iteration.
  if (_acceleration || (_use_initial_flux && _iteration_postprocessor == 1.0))
    return ArrayKernel::computeQpOffDiagJacobian(jvar);
  else
  {
    RealEigenVector lhs = _tau_sn[_qp][_group] * _ordinates * _array_grad_test[_i][_qp] +
      RealEigenVector::Constant(_count, _test[_i][_qp]);

    for (unsigned int g = 0; g < _num_groups; ++g)
      if (jvar.number() == _flux_ids[g])
      {
        RealEigenMatrix jac = RealEigenMatrix::Zero(_count, _count);

        for (unsigned int i = 0; i < _count; ++i)
          for (unsigned int j = 0; j < _count; ++j)
            jac(i, j) -= _ls_norm_factor * _weights(i) * lhs(i) * _nsf[_qp][g] * _phi[_j][_qp] *
              _weights(j);

        if (_account_delayed)
          jac *= (1. - _beta[_qp]) * _chi_p[_qp][_group];
        else
          jac *= _chi_t[_qp][_group];

        if (_eigenvalue_scaling != 1.0)
          jac /= _eigenvalue_scaling;

        return jac;
      }
    return ArrayKernel::computeQpOffDiagJacobian(jvar);
  }
}
