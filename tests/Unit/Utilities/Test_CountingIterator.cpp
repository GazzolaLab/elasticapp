//
// Created by Tejaswin Parthasarathy on 3/23/22.
//

#include <algorithm>
#include <array>

#include "Framework/TestingFramework.hpp"
#include "Utilities/CountingIterator.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.CountingIterator", "[Unit][Utilities]") {
    /// [counting_iterator_eg]
    constexpr std::size_t n_values = 5UL;
    using value_type = int;
    std::array<value_type, 5> data = {1, 3, 5, 7, 9};
    value_type counter = 0;
    std::for_each(elastica::make_counting_iterator(0UL),
                  elastica::make_counting_iterator(n_values),
                  [&counter, &data](std::size_t idx) { counter += data[idx]; });
    /// [counting_iterator_eg]
    CHECK(counter == 25);
  }

}  // namespace
