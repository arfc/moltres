#!/bin/bash
# run this script with one or more arguments that are the directories that hold the code files for
# the objects you want to update.

dirs=$@

appname='MoltresApp'

objs='
DelayedNeutronSource
DelayedNeutronEigenSource
SigmaR
GammaHeatSource
ScalarTransportTimeDerivative
PrecursorSource
PrecursorDecay
CoupledScalarAdvection
DivFreeCoupledScalarAdvection
NtTimeDerivative
FissionHeatSource
TransientFissionHeatSource
InScatter
CoupledFissionEigenKernel
ScalarAdvectionArtDiff
CoupledFissionKernel
SelfFissionEigenKernel
INSMomentumKEpsilon
INSBoussinesqBodyForce
GroupDiffusion
ManuHX
LinLogPenaltyDirichletBC
ScalarAdvectionArtDiffNoBCBC
VacuumConcBC
ConservativeAdvectionNoBCBC
DiffusionNoBCBC
CoupledScalarAdvectionNoBCBC
INSOutflowBC
INSSymmetryAxisBC
MatDiffusionFluxBC
GenericMoltresMaterial
CammiFuel
CammiModerator
Nusselt
GraphiteTwoGrpXSFunctionMaterial
MsreFuelTwoGrpXSFunctionMaterial
RoddedMaterial
LimitK
IntegralOldVariablePostprocessor
ElementL2Diff
IntegralNewVariablePostprocessor
DivisionPostprocessor
ElmIntegTotFissPostprocessor
ElmIntegTotFissHeatPostprocessor
ElmIntegTotFissNtsPostprocessor
AverageFissionHeat
FissionHeatSourceAux
FissionHeatSourceTransientAux
ModeratorHeatSourceTransientAux
MatDiffusionAux
ConstantDifferenceAux
DiracHX
'

actions='
PrecursorAction add_kernel
PrecursorAction add_postprocessor
PrecursorAction add_bc
PrecursorAction add_variable
PrecursorAction add_ic
PrecursorAction add_dg_kernel
PrecursorAction add_transfer
PrecursorAction check_copy_nodal_vars
PrecursorAction copy_nodal_vars
NtAction add_kernel
NtAction add_bc
NtAction add_variable
NtAction add_ic
NtAction add_aux_variable
NtAction add_aux_kernel
NtAction check_copy_nodal_vars
NtAction copy_nodal_vars
'

for obj in $objs; do
    find $dirs | grep "/${obj}\.C$" | xargs perl -0777 -i'' -pe 's/(template ?<>\nInputParameters\n? ?validParams<(\w*)>\(\))/registerMooseObject\("'"$appname"'", \2\);\n\n\1/igs'
done

while read action task; do
    find $dirs | grep "/${action}\.C$" | xargs perl -0777 -i'' -pe 's/(template ?<>\nInputParameters\n? ?validParams<(\w*)>\(\))/registerMooseAction\("'"$appname"'", \2, "'"$task"'"\);\n\n\1/igs'
done <<< "$actions"
