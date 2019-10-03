// Distributed under the MIT License.
// See LICENSE.txt for details.

#define CATCH_CONFIG_RUNNER
#include <cstdio>
#include "catch.hpp"

int main(int argc, char* argv[]) {
  // global setup...

  int result = Catch::Session().run(argc, argv);

  // global clean-up...

  return result;
}

// [[OutputRegex, I failed]]
TEST_CASE("TestFramework.Hello", "[Unit]") { printf("I failed"); }

// int main() { return 0; }
