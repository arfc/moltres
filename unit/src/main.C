
// CPPUnit includes
#include "cppunit/XmlOutputter.h"
#include "cppunit/CompilerOutputter.h"
#include "cppunit/ui/text/TestRunner.h"
#include "cppunit/extensions/TestFactoryRegistry.h"

// Moose includes
#include "Moose.h"
#include "MooseInit.h"

#include "Factory.h"
#include "AppFactory.h"
#include "MoltresApp.h"

#include <fstream>
#include <string>

PerfLog Moose::perf_log("CppUnit");

int
main(int argc, char ** argv)
{
  MooseInit init(argc, argv);

  registerApp(MoltresApp);

  CppUnit::Test * suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

  CppUnit::TextTestRunner runner;
  runner.addTest(suite);
  std::ofstream out;

  // If you run with --xml, output will be sent to an xml file instead of the screen
  if (argc == 2 && std::string(argv[1]) == std::string("--xml"))
  {
    runner.setOutputter(new CppUnit::XmlOutputter(&runner.result(), out));
    out.open("test_results.xml");
  }

  else
  {
    // Note: upon calling setOutputter, any previous outputter is
    // destroyed. The TextTestRunner assumes ownership of the outputter, so you
    // don't have to worry about deleting it.
    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), Moose::err));
  }

  bool wasSucessful = runner.run(/*testPath=*/"",
                                 /*doWait=*/false,
                                 /*doPrintResult=*/true,
                                 /*doPrintProgress=*/false);

  return wasSucessful ? 0 : 1;
}
