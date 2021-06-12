#pragma once

#include "ElmIntegTotFissPostprocessor.h"

class ElmIntegTotFissHeatPostprocessor : public ElmIntegTotFissPostprocessor
{
public:
  ElmIntegTotFissHeatPostprocessor(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeFluxMultiplier(int index) override;

  const MaterialProperty<std::vector<Real>> & _fisse;
};
