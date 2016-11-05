#include "MoltresApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "NavierStokesApp.h"
#include "FluidPropertiesApp.h"
#include "HeatConductionApp.h"
#include "MooseTestApp.h"
#include "MooseSyntax.h"

// Kernels
#include "PrecursorDecay.h"
#include "NtTimeDerivative.h"
#include "PrecursorSource.h"
#include "FissionHeatSource.h"
#include "INSMomentumKEpsilon.h"
#include "INSK.h"
#include "SigmaR.h"
#include "CoupledFissionEigenKernel.h"
#include "SelfFissionEigenKernel.h"
#include "InScatter.h"
#include "GroupDiffusion.h"
#include "CoupledScalarAdvection.h"

// Boundary conditions
#include "INSOutflowBC.h"
#include "INSSymmetryAxisBC.h"
#include "MatDiffusionFluxBC.h"

// Materials
#include "GenericMoltresMaterial.h"

// Postprocessors
#include "ElmIntegTotFissPostprocessor.h"
#include "ElmIntegTotFissNtsPostprocessor.h"

//AuxKernels
#include "FissionHeatSourceAux.h"
#include "MatDiffusionAux.h"

template<>
InputParameters validParams<MoltresApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

MoltresApp::MoltresApp(InputParameters parameters) :
    MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  NavierStokesApp::registerObjects(_factory);
  FluidPropertiesApp::registerObjects(_factory);
  HeatConductionApp::registerObjects(_factory);
  MooseTestApp::registerObjects(_factory);
  MoltresApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  NavierStokesApp::associateSyntax(_syntax, _action_factory);
  FluidPropertiesApp::associateSyntax(_syntax, _action_factory);
  HeatConductionApp::associateSyntax(_syntax, _action_factory);
  MooseTestApp::associateSyntax(_syntax, _action_factory);
  MoltresApp::associateSyntax(_syntax, _action_factory);
}

MoltresApp::~MoltresApp()
{
}

// External entry point for dynamic application loading
extern "C" void MoltresApp__registerApps() { MoltresApp::registerApps(); }
void
MoltresApp::registerApps()
{
  registerApp(MoltresApp);
}

// External entry point for dynamic object registration
extern "C" void MoltresApp__registerObjects(Factory & factory) { MoltresApp::registerObjects(factory); }
void
MoltresApp::registerObjects(Factory & factory)
{
  registerKernel(SigmaR);
  registerKernel(PrecursorSource);
  registerKernel(PrecursorDecay);
  registerKernel(CoupledScalarAdvection);
  registerKernel(NtTimeDerivative);
  registerKernel(FissionHeatSource);
  registerKernel(InScatter);
  registerKernel(CoupledFissionEigenKernel);
  registerKernel(SelfFissionEigenKernel);
  registerKernel(INSMomentumKEpsilon);
  registerKernel(INSK);
  registerKernel(GroupDiffusion);
  registerBoundaryCondition(INSOutflowBC);
  registerBoundaryCondition(INSSymmetryAxisBC);
  registerBoundaryCondition(MatDiffusionFluxBC);
  registerMaterial(GenericMoltresMaterial);
  registerPostprocessor(ElmIntegTotFissPostprocessor);
  registerPostprocessor(ElmIntegTotFissNtsPostprocessor);
  registerAux(FissionHeatSourceAux);
  registerAux(MatDiffusionAux);
}

// External entry point for dynamic syntax association
extern "C" void MoltresApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { MoltresApp::associateSyntax(syntax, action_factory); }
void
MoltresApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
