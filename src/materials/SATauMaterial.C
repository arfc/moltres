#include "SATauMaterial.h"
#include "INSADTauMaterial.h"
#include "INSADMaterial.h"

registerMooseObject("MoltresApp", SATauMaterial);

template class SATauMaterialTempl<INSADTauMaterial>;
