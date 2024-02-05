#include "MoltresApp.h"
#include "MooseMain.h"

// Begin the main program.
int
main(int argc, char * argv[])
{
  Moose::main<MoltresApp>(argc, argv);

  return 0;
}
