#pragma once

#include "AuxKernel.h"
#include "ScalarTransportBase.h"

class GroupDriftAux : public ArrayAuxKernel, public ScalarTransportBase
{
public:
  static InputParameters validParams();

  GroupDriftAux(const InputParameters & parameters);

protected:
  virtual RealEigenVector computeValue() override;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _tau_sn;
  const MaterialProperty<std::vector<Real>> & _diffcoef;
  const MaterialProperty<std::vector<Real>> & _totxs;
  const MaterialProperty<std::vector<Real>> & _scatter;

  /// Neutron group number
  const unsigned int _group;

  /// Number of neutron groups
  const unsigned int _num_groups;

  /// Whether to set a diffusion coefficient limit
  const bool _set_limit;

  /// Diffusion coefficient limit value
  const Real _limit;

  /// Group flux variables
  std::vector<const ArrayVariableValue *> _group_fluxes;

  /// Group flux variable gradients
  std::vector<const ArrayVariableGradient *> _grad_group_fluxes;

  /// Level-symmetric quadrature points
  RealEigenMatrix _ordinates;

  /// Level-symmetric quadrature weights
  RealEigenVector _weights;
};
