
#include "gtest/gtest.h"

// Moose includes
#include "Moose.h"
#include "MooseInit.h"

#include "Factory.h"
#include "AppFactory.h"
#include "MoltresApp.h"

#include <fstream>
#include <string>

PerfLog Moose::perf_log("gtest");

GTEST_API_ int
main(int argc, char ** argv)
{
  // gtest removes (only) its args from argc and argv - so this must be before moose init
  testing::InitGoogleTest(&argc, argv);

  MooseInit init(argc, argv);
  registerApp(MoltresApp);
  Moose::_throw_on_error = true;

  return RUN_ALL_TESTS();
}
