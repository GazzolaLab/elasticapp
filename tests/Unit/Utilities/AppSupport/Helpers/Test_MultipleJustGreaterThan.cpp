//
// Created by Tejaswin Parthasarathy on 2/3/22.
//
#include <random>

#include "Framework/TestingFramework.hpp"
#include "Utilities/AppSupport/Helpers/MultipleJustGreaterThan.hpp"
#include "Utilities/DefineTypes.h"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.AppSupport.MultipleJustGreaterThan",
                     "[Unit][Utilities]") {
    using ::elastica::multiple_just_greater_than;

    // the most problematic boys first
    CHECK(static_cast<std::size_t>(multiple_just_greater_than(0.99, 1e-5)) ==
          99000UL);
    CHECK(static_cast<std::size_t>(multiple_just_greater_than(1.0, 1e-5)) ==
          100000UL);
    CHECK(static_cast<std::size_t>(multiple_just_greater_than(1.01, 1e-5)) ==
          101000UL);

    std::random_device random_dev;
    std::mt19937 generator(random_dev());
    std::uniform_int_distribution<std::size_t> distr(100UL, 100000UL);
    std::size_t n_samples(10UL);

    std::vector<std::size_t> expected_n_steps(n_samples);
    std::generate(std::begin(expected_n_steps), std::end(expected_n_steps),
                  [&]() { return distr(generator); });

    std::vector<std::size_t> actual_n_steps(n_samples);
    std::transform(std::begin(expected_n_steps), std::end(expected_n_steps),
                   std::begin(actual_n_steps), [](std::size_t n_step) {
                     const ::elastica::real_t dt = 3.24e-4;
                     const ::elastica::real_t final_time =
                         static_cast<::elastica::real_t>(n_step) * dt;
                     return static_cast<std::size_t>(
                         multiple_just_greater_than(final_time, dt));
                   });
    CHECK(actual_n_steps == expected_n_steps);
  }

}  // namespace