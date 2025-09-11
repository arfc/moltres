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
  const unsigned int _group;
  const unsigned int _num_groups;
  const int _L;
  const bool _acceleration;
  const bool _use_initial_flux;
  const PostprocessorValue & _iteration_postprocessor;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<const ArrayVariableValue *> _group_angular_fluxes;
  std::vector<unsigned int> _flux_ids;
  RealEigenMatrix _ordinates;
  RealEigenVector _weights;
  RealEigenMatrix _harmonics;
};
