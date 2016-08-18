#ifndef MAGMARAPP_H
#define MAGMARAPP_H

#include "MooseApp.h"

class MagmarApp;

template<>
InputParameters validParams<MagmarApp>();

class MagmarApp : public MooseApp
{
public:
  MagmarApp(InputParameters parameters);
  virtual ~MagmarApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* MAGMARAPP_H */
