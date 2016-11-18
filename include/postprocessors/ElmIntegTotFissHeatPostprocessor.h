#ifndef ELMINTEGTOTFISSHEATPOSTPROCESSOR_H
#define ELMINTEGTOTFISSHEATPOSTPROCESSOR_H

#include "ElmIntegTotFissPostprocessor.h"

//Forward Declarations
class ElmIntegTotFissHeatPostprocessor;

template<>
InputParameters validParams<ElmIntegTotFissHeatPostprocessor>();

class ElmIntegTotFissHeatPostprocessor :
  public ElmIntegTotFissPostprocessor
{
public:
  ElmIntegTotFissHeatPostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeFluxMultiplier(int index) override;

  const MaterialProperty<std::vector<Real> > & _fisse;
};

#endif
