//
// Created by Tejaswin Parthasarathy on 1/12/21.
//

#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Utilities/Size.hpp"

/// [size_example_setup]

namespace custom {
  struct Custom {
    // does not have a size() member function
    std::size_t customised_size() const noexcept { return 5UL; }
  };

  // so we define a free function in its namespace
  auto size(Custom const& c) -> std::size_t { return c.customised_size(); }
}  // namespace custom

/// [size_example_setup]

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.Size.size", "[Unit][Utilities]") {
    /// [size_eg]
    auto a = std::vector<int>{1, 2, 3, 4, 5};
    CHECK(std::size(a) == 5UL);
    /// [size_eg]

    /// [size_example]
    auto b = std::vector<int>{1, 2, 3};
    // instead of doing two-phase lookup, or std::size,
    // call elastica::size
    CHECK(elastica::size(b) == 3UL);
    /// [size_example]

    /// [size_example_custom]
    custom::Custom c{};
    // instead of doing two-phase lookup, call elastica::size
    // wroks with custom types too, that doesn't have a size() member
    CHECK(elastica::size(c) == 5UL);
    /// [size_example_custom]
  }

}  // namespace
