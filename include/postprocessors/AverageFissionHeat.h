#pragma once

#include "ElmIntegTotFissHeatPostprocessor.h"

class AverageFissionHeat : public ElmIntegTotFissHeatPostprocessor
{
public:
  AverageFissionHeat(const InputParameters & parameters);

  static InputParameters validParams();

  using Postprocessor::getValue;
  virtual Real getValue() const override;

protected:
  virtual void initialize() override;
  virtual void execute() override;
  virtual void threadJoin(const UserObject & y) override;
  virtual void finalize() override;

  Real _volume;
};
