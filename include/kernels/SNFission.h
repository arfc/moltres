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

  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _nsf;
  const MaterialProperty<std::vector<Real>> & _d_nsf_d_temp;
  const MaterialProperty<std::vector<Real>> & _chi_t;
  const MaterialProperty<std::vector<Real>> & _d_chi_t_d_temp;
  const MaterialProperty<std::vector<Real>> & _chi_p;
  const MaterialProperty<std::vector<Real>> & _d_chi_p_d_temp;
  const MaterialProperty<Real> & _beta;
  const MaterialProperty<Real> & _d_beta_d_temp;
  const unsigned int _N;
  const unsigned int _group;
  const unsigned int _num_groups;
  const unsigned int _temp_id;
  const bool _account_delayed;
  const PostprocessorValue & _eigenvalue_scaling;
  std::vector<const ArrayVariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
