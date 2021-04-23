#pragma once

#include "ElmIntegTotFissHeatPostprocessor.h"

class AverageFissionHeat : public ElmIntegTotFissHeatPostprocessor
{
public:
  AverageFissionHeat(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void initialize() override;
  virtual void execute() override;
  virtual Real getValue() override;
  virtual void threadJoin(const UserObject & y) override;

  Real _volume;
};
