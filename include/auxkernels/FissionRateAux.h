#pragma once

#include "AuxKernel.h"

/**
 * Computes heat source due to fission during a transient.
 * This is the same as FissionHeatSourceAux, but with the exception
 * that the power is not normalized to some user-defined value. The reactor
 * will produce heat freely. You'll probably see thermal feedback since MSRs are
 * nice like that.
 */
class FissionRateAux : public AuxKernel
{
public:
  FissionRateAux(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue() override;

  // Group constant MaterialProperty(s)
  const MaterialProperty<std::vector<Real>> & _fissxs;

  /// Number of neutron groups
  unsigned int _num_groups;

  /// Neutron flux scaling factor
  Real _nt_scale;

  /// Group flux variables
  std::vector<const VariableValue *> _group_fluxes;
};
