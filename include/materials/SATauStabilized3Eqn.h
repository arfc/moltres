#pragma once

#include "SATauMaterial.h"
#include "INSADStabilized3Eqn.h"

class INSADStabilized3Eqn;

class SATauStabilized3Eqn : public SATauMaterialTempl<INSADStabilized3Eqn>
{
public:
  static InputParameters validParams();

  SATauStabilized3Eqn(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  const Real _pr;

  using SATauMaterialTempl<INSADStabilized3Eqn>::_mu_tilde;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_grad_mu;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_mu;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_k;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_rho;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_cp;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_alpha;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_temperature_strong_residual;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_has_energy_transient;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_grad_temperature;
  using SATauMaterialTempl<INSADStabilized3Eqn>::_second_temperature;
};
