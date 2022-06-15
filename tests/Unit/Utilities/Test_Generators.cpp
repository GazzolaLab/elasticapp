//
// Created by Tejaswin Parthasarathy on 11/7/21.
//

#include <type_traits>
#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Utilities/Generators.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {

  using namespace elastica;

  template <typename F>
  auto pass_as_std_generator(F f) noexcept {
    return [i = 0UL, f = std::move(f)]() mutable { return f(i++); };
  }

  ELASTICA_TEST_CASE("Unit.Utilities.linspace_generator", "[Utilities][Unit]") {
    SECTION("Check throws for zero steps") {
      CHECK_THROWS_WITH(linspace_generator(0.0, 1.0, 0),
                        Catch::Contains("Cannot create"));
    }
    SECTION("Correctness for one step") {
      const std::size_t index = 0UL;
      CHECK(linspace_generator(5.0, 10.0, 1UL)(index) == approx(5.0));
    }
    SECTION("Correctness for two steps") {
      const std::size_t zero = 0UL;
      const std::size_t one = 1UL;

      SECTION("Endpoint included") {
        CHECK(linspace_generator(5.0, 10.0, 2UL, true)(zero) == approx(5.0));
        CHECK(linspace_generator(5.0, 10.0, 2UL, true)(one) == approx(10.0));
      }
      SECTION("Endpoint excluded") {
        CHECK(linspace_generator(5.0, 10.0, 2UL, false)(zero) == approx(5.0));
        CHECK(linspace_generator(5.0, 10.0, 2UL, false)(one) == approx(7.5));
      }
    }
    SECTION("Correctness for multiple steps") {
      SECTION("Endpoint included") {
        std::size_t n_points(21UL);
        std::vector<double> data(n_points);
        std::generate(data.begin(), data.end(),
                      pass_as_std_generator(
                          linspace_generator(5.0, 10.0, n_points, true)));
        CHECK(data[0UL] == approx(5.0));
        CHECK(data[1UL] == approx(5.25));
        CHECK(data[n_points - 1UL] == approx(10.0));
      }
      SECTION("Endpoint excluded") {
        std::size_t n_points(20UL);
        std::vector<double> data(n_points);
        std::generate(data.begin(), data.end(),
                      pass_as_std_generator(
                          linspace_generator(5.0, 10.0, n_points, false)));
        CHECK(data[0UL] == approx(5.0));
        CHECK(data[1UL] == approx(5.25));
        CHECK(data[n_points - 1UL] == approx(9.75));
      }
      {
        // [linspace_eg]
        // generates the sequence upon invocation [2.  , 2.25, 2.5 , 2.75, 3. ]
        auto lg = ::elastica::linspace_generator(2.0, 3.0, 5UL);
        // i.e. lg(2UL) == 2.5

        // generates the sequence ([2. ,  2.2,  2.4,  2.6,  2.8]) upon
        // invocation
        auto alg = ::elastica::linspace_generator(2.0, 3.0, 5UL, false);
        // i.e. lg(2UL) == 2.4
        // [linspace_eg]

        std::size_t n_points(5UL);
        std::vector<double> data(n_points);
        std::generate(data.begin(), data.end(),
                      pass_as_std_generator(std::move(lg)));
        std::vector<double> expected{2., 2.25, 2.5, 2.75, 3.};
        CHECK_ITERABLE_APPROX(data, expected);

        std::generate(data.begin(), data.end(),
                      pass_as_std_generator(std::move(alg)));
        std::vector<double> aexpected{2., 2.2, 2.4, 2.6, 2.8};
        CHECK_ITERABLE_APPROX(data, aexpected);
      }

    }  // namespace
    SECTION("Check throws out of range") {
      std::size_t n_points(20UL);
      auto gen = linspace_generator(5.0, 10.0, n_points, false);
      CHECK_NOTHROW(gen(0UL));
      CHECK_NOTHROW(gen(5UL));
      CHECK_NOTHROW(gen(10UL));
      CHECK_NOTHROW(gen(15UL));
      CHECK_THROWS_WITH(gen(20UL), Catch::Contains("out of range"));
    }
    SECTION("Correctness for Blaze vectors") {
      std::size_t n_points(3UL);
      const Vec3 start{0.0, 0.0, 0.0};
      const Vec3 end{1.0, 0.0, 0.0};

      auto gen = linspace_generator(start, end, n_points, true);
      CHECK(gen(0UL) == start);
      CHECK(gen(1UL) == Vec3{0.5, 0.0, 0.0});
      CHECK(gen(2UL) == end);

      auto another_gen = linspace_generator(start, end, 1UL, true);
      CHECK(another_gen(0UL) == start);
    }
  }

}  // namespace
