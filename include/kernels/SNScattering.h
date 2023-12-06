#pragma once

#include "ArrayKernel.h"

class SNScattering : public ArrayKernel
{
public:
  static InputParameters validParams();

  SNScattering(const InputParameters & parameters);

protected:
  virtual void computeQpResidual(RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian() override;
  virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;

  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _scatter;
  const MaterialProperty<std::vector<Real>> & _d_scatter_d_temp;
  const unsigned int _N;
  const unsigned int _group;
  const unsigned int _num_groups;
  const int _L;
  const unsigned int _temp_id;
  std::vector<const ArrayVariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
};
