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
  return params;
}

// When using the new Registry system, this line is required so that
// dependent apps know about the MoltresApp label.
registerKnownLabel("MoltresApp");

MoltresApp::MoltresApp(const InputParameters & parameters)
  : MooseApp(parameters)
{
  MoltresApp::registerAll(_factory, _action_factory, _syntax);
}

MoltresApp::~MoltresApp() {}

void
MoltresApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAllObjects<MoltresApp>(f, af, s);
  Registry::registerObjectsTo(f, {"MoltresApp"});
  Registry::registerActionsTo(af, {"MoltresApp"});

  // register objects from Squirrel
  SquirrelApp::registerAll(f, af, s);

  // register custom execute flags, action syntax, etc. here
  s.registerActionSyntax("NtAction", "Nt");
  s.registerActionSyntax("PrecursorAction", "Precursors/*");
}

void
MoltresApp::registerApps()
{
  registerApp(MoltresApp);
  ModulesApp::registerApps();
}

/**************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY *****************************
 *************************************************************************************************/

// External entry point for object registration
extern "C" void
MoltresApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  MoltresApp::registerAll(f, af, s);
}

// External entry point for dynamic application loading
extern "C" void
MoltresApp__registerApps()
{
  MoltresApp::registerApps();
}
