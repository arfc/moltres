#ifndef AVERAGEFISSIONHEAT_H
#define AVERAGEFISSIONHEAT_H

#include "ElmIntegTotFissHeatPostprocessor.h"

//Forward Declarations
class AverageFissionHeat;

template<>
InputParameters validParams<AverageFissionHeat>();

class AverageFissionHeat : public ElmIntegTotFissHeatPostprocessor
{
public:
  AverageFissionHeat(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;
  virtual Real getValue() override;
  virtual void threadJoin(const UserObject & y) override;

protected:
  Real _volume;
};

#endif
