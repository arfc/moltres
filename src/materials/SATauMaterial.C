#include "SATauMaterial.h"
#include "INSADTauMaterial.h"

registerMooseObject("MoltresApp", SATauMaterial);

template class SATauMaterialTempl<INSADTauMaterial>;
