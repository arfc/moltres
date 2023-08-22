#include "AverageFissionHeat.h"

registerMooseObject("MoltresApp", AverageFissionHeat);

InputParameters
AverageFissionHeat::validParams()
{
  InputParameters params = ElmIntegTotFissHeatPostprocessor::validParams();
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
AverageFissionHeat::getValue() const
{
  return _integral_value / _volume;
}

void
AverageFissionHeat::threadJoin(const UserObject & y)
{
  ElmIntegTotFissHeatPostprocessor::threadJoin(y);
  const AverageFissionHeat & pps = static_cast<const AverageFissionHeat &>(y);
  _volume += pps._volume;
}

void
AverageFissionHeat::finalize()
{
  gatherSum(_volume);
  gatherSum(_integral_value);
}
