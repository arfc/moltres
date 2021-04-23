#pragma once

#include "AuxKernel.h"

/**
 * computes the heating term due to fissions.
 * \f[
 *   \dot{Q} = \sum_g \phi_g \Sigma_{g,f} q_{fiss}
 * \f]
 * Where \f$ q_{fiss} \f$ is the average heat produced per fission.
 * Note that in particular, this kernel is not meant for transients and instead is for
 * specifying the power through the "power" parameter.
 */
class FissionHeatSourceAux : public AuxKernel
{
public:
  FissionHeatSourceAux(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue() override;

  const MaterialProperty<std::vector<Real>> & _fissxs;
  const MaterialProperty<std::vector<Real>> & _fisse;
  unsigned int _num_groups;
  const PostprocessorValue & _tot_fission_heat;
  Real _power;
  std::vector<const VariableValue *> _group_fluxes;
  std::vector<unsigned int> _flux_ids;
};
