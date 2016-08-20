#include "MagmarApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

// Kernels
#include "INSMomentumKEpsilon.h"

template<>
InputParameters validParams<MagmarApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

MagmarApp::MagmarApp(InputParameters parameters) :
    MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  MagmarApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  MagmarApp::associateSyntax(_syntax, _action_factory);
}

MagmarApp::~MagmarApp()
{
}

// External entry point for dynamic application loading
extern "C" void MagmarApp__registerApps() { MagmarApp::registerApps(); }
void
MagmarApp::registerApps()
{
  registerApp(MagmarApp);
}

// External entry point for dynamic object registration
extern "C" void MagmarApp__registerObjects(Factory & factory) { MagmarApp::registerObjects(factory); }
void
MagmarApp::registerObjects(Factory & factory)
{
  registerKernel(INSMomentumKEpsilon);
}

// External entry point for dynamic syntax association
extern "C" void MagmarApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { MagmarApp::associateSyntax(syntax, action_factory); }
void
MagmarApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
