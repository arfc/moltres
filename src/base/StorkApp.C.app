#include "StorkApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<StorkApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

StorkApp::StorkApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  StorkApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  StorkApp::associateSyntax(_syntax, _action_factory);
}

StorkApp::~StorkApp()
{
}

void
StorkApp::registerApps()
{
  registerApp(StorkApp);
}

void
StorkApp::registerObjects(Factory & factory)
{
}

void
StorkApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
