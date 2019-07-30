// Distributed under the MIT License.
// See LICENSE.txt for details.

#define CATCH_CONFIG_MAIN
#include <cstdio>
#include "catch.hpp"

// [[OutputRegex, I failed]]
TEST_CASE("TestFramework.Hello", "[Unit]") { printf("I failed"); }

// int main() { return 0; }
