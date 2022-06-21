//
// Created by Tejaswin Parthasarathy on 5/30/22.
//

#include <string>
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Utilities/ConvertCase/ConvertCase.hpp"

namespace {
  using namespace convert_case;

  ELASTICA_TEST_CASE("Unit.Utilities.ConvertCase.PascalToSnake",
                     "[Unit][Utilities]") {
    using TestSet = std::pair<std::string, std::string>;
    auto test_sets = {TestSet{"This", "this"}, TestSet{"ThisIs", "this_is"},
                      TestSet{"ThisIsA", "this_is_a"},
                      TestSet{"ThisIsAString", "this_is_a_string"},
                      TestSet{"_This", "_this"}};
    for (auto test_set : test_sets) {
      CHECK(convert(test_set.first, FromPascalCase{}, ToSnakeCase{}) ==
            test_set.second);
    }
  }

}  // namespace