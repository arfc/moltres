#pragma once

#include "MooseApp.h"

class MoltresApp : public MooseApp
{
public:
  MoltresApp(InputParameters parameters);

  static InputParameters validParams();

  virtual ~MoltresApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s);
};
