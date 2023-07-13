#include "CDTauMaterial.h"
#include "SATauMaterial.h"
#include "INSADTauMaterial.h"
#include "INSADMaterial.h"

registerMooseObject("MoltresApp", CDTauMaterial);

template class CDTauMaterialTempl<SATauMaterial>;
