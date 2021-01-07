// Distributed under the MIT License.
// See LICENSE.txt for details.

#define CATCH_CONFIG_RUNNER
#include <cstdio>

#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "RunTestsRegister.hpp"  // Auto-generated file
#include "catch.hpp"

int main(int argc, char* argv[]) {
  // global setup...
  register_run_tests_libs();  // From RunTestsRegister
  enable_floating_point_exceptions();

  int result = Catch::Session().run(argc, argv);

  // global clean-up...

  return result;
}

// [[OutputRegex, I passed]]
ELASTICA_TEST_CASE("TestFramework.Hello", "[Unit]") {
  OUTPUT_TEST();
  printf("I failed");
}
