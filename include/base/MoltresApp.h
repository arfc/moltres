#ifndef MAGMARAPP_H
#define MAGMARAPP_H

#include "MooseApp.h"

class MoltresApp;

template <>
InputParameters validParams<MoltresApp>();

class MoltresApp : public MooseApp
{
public:
  MoltresApp(InputParameters parameters);
  virtual ~MoltresApp();

  static void registerApps();
  static void registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* MAGMARAPP_H */
