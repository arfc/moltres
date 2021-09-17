#include "MoltresApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "SquirrelApp.h"
#include "MooseSyntax.h"

InputParameters
MoltresApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

// When using the new Registry system, this line is required so that
// dependent apps know about the MoltresApp label.
registerKnownLabel("MoltresApp");

MoltresApp::MoltresApp(InputParameters parameters)
  : MooseApp(parameters)
{
  MoltresApp::registerAll(_factory, _action_factory, _syntax);
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

// External entry point for object registration
extern "C" void
MoltresApp__registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax)
{
  MoltresApp::registerAll(factory, action_factory, syntax);
}
void
MoltresApp::registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax)
{
  Registry::registerObjectsTo(factory, {"MoltresApp"});
  Registry::registerActionsTo(action_factory, {"MoltresApp"});
  MoltresApp::associateSyntax(syntax, action_factory);

  ModulesApp::registerAll(factory, action_factory, syntax);
  SquirrelApp::registerAll(factory, action_factory, syntax);
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
