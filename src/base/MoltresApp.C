#include "MoltresApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "SquirrelApp.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<MoltresApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

MoltresApp::MoltresApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  SquirrelApp::registerObjects(_factory);
  MoltresApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  SquirrelApp::associateSyntax(_syntax, _action_factory);
  MoltresApp::associateSyntax(_syntax, _action_factory);
}

MoltresApp::~MoltresApp() {}

// External entry point for dynamic application loading
extern "C" void
MoltresApp__registerApps()
{
  MoltresApp::registerApps();
}
void
MoltresApp::registerApps()
{
  registerApp(MoltresApp);
}

// External entry point for dynamic object registration
extern "C" void
MoltresApp__registerObjects(Factory & factory)
{
  MoltresApp::registerObjects(factory);
}
void
MoltresApp::registerObjects(Factory & factory)
{
  Registry::registerObjectsTo(factory, {"MoltresApp"});
}

// External entry point for dynamic syntax association
extern "C" void
MoltresApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  MoltresApp::associateSyntax(syntax, action_factory);
}
void
MoltresApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  Registry::registerActionsTo(action_factory, {"MoltresApp"});
  syntax.registerActionSyntax("PrecursorAction", "Precursors/*");
  syntax.registerActionSyntax("NtAction", "Nt");
}
