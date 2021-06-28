#pragma once

#include "MooseApp.h"

class MoltresApp : public MooseApp
{
public:
  MoltresApp(InputParameters parameters);

  static InputParameters validParams();

  virtual ~MoltresApp();

  static void registerApps();
  static void registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};
