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

  /// Level-symmetric quadrature normalization factor for isotropic source
  const Real _ls_norm_factor = 0.125;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _scatter;

  /// Neutron group number
  const unsigned int _group;

  /// Number of neutron groups
  const unsigned int _num_groups;

  /// Discrete ordinates order
  const int _L;

  /// Whether the nonlinear diffusion acceleration scheme is being applied
  const bool _acceleration;

  /// Whether to use the diffusion flux as the initial condition in the hybrid method
  const bool _use_initial_flux;

  /// Fixed point iteration number
  const PostprocessorValue & _iteration_postprocessor;

  /// Group flux variables
  std::vector<const VariableValue *> _group_fluxes;

  /// Angular group flux variables
  std::vector<const ArrayVariableValue *> _group_angular_fluxes;

  /// Angular group flux variable IDs
  std::vector<unsigned int> _flux_ids;

  /// Level-symmetric quadrature points
  RealEigenMatrix _ordinates;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;

  /// Level-symmetric quadrature harmonics
  RealEigenMatrix _harmonics;
};
