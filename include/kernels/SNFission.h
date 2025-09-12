#pragma once

#include "ArrayKernel.h"
#include "Eigen/Core"

class SNFission : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNFission(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;
  virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;

  const Real _ls_norm_factor = 0.125;
  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _nsf;
  const MaterialProperty<std::vector<Real>> & _chi_t;
  const MaterialProperty<std::vector<Real>> & _chi_p;
  const MaterialProperty<Real> & _beta;
  const unsigned int _group;
  const unsigned int _num_groups;
  const bool _account_delayed;
  const PostprocessorValue & _eigenvalue_scaling;
  const bool _acceleration;
  const bool _use_initial_flux;
  const PostprocessorValue & _iteration_postprocessor;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<const ArrayVariableValue *> _group_angular_fluxes;
  std::vector<unsigned int> _flux_ids;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
