#pragma once

#include "ArrayKernel.h"
#include "Eigen/Core"

class SNTransport : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNTransport(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;
  virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;

  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _totxs;
  const MaterialProperty<std::vector<Real>> & _d_totxs_d_temp;
  const MaterialProperty<std::vector<Real>> & _scatter;
  const MaterialProperty<std::vector<Real>> & _d_scatter_d_temp;
  const unsigned int _N;
  const unsigned int _group;
  const unsigned int _num_groups;
  const bool _has_scatter;
  const bool _has_ext_src;
  std::vector<const Function *> _func;
  const int _L;
  const unsigned int _temp_id;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
  std::vector<const ArrayVariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
};
