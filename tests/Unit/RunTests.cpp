// Distributed under the MIT License.
// See LICENSE.txt for details.

#define CATCH_CONFIG_RUNNER
#include <cstdio>
#include "catch.hpp"

#include "tests/Unit/TestingFramework.hpp"
#include "tests/Unit/RunTestsRegister.hpp" // Auto-generated file

int main(int argc, char* argv[]) {
  // global setup...
  register_run_tests_libs(); // From RunTestsRegister

  int result = Catch::Session().run(argc, argv);

  // global clean-up...

  return result;
}

// [[OutputRegex, I failed]]
ELASTICA_TEST_CASE("TestFramework.Hello", "[Unit]") { printf("I failed"); }
