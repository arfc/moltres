#include "AverageFissionHeat.h"

registerMooseObject("MoltresApp", AverageFissionHeat);

template <>
InputParameters
validParams<AverageFissionHeat>()
{
  InputParameters params = validParams<ElmIntegTotFissHeatPostprocessor>();
  return params;
}

AverageFissionHeat::AverageFissionHeat(const InputParameters & parameters)
  : ElmIntegTotFissHeatPostprocessor(parameters), _volume(0)
{
}

void
AverageFissionHeat::initialize()
{
  ElmIntegTotFissHeatPostprocessor::initialize();
  _volume = 0;
}

void
AverageFissionHeat::execute()
{
  ElmIntegTotFissHeatPostprocessor::execute();

  _volume += _current_elem_volume;
}

Real
AverageFissionHeat::getValue()
{
  Real integral = ElmIntegTotFissHeatPostprocessor::getValue();

  gatherSum(_volume);

  return integral / _volume;
}

void
AverageFissionHeat::threadJoin(const UserObject & y)
{
  ElmIntegTotFissHeatPostprocessor::threadJoin(y);
  const AverageFissionHeat & pps = static_cast<const AverageFissionHeat &>(y);
  _volume += pps._volume;
}
