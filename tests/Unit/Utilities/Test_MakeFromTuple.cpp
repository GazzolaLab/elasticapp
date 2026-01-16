//
// Created by Tejaswin Parthasarathy on 6/8/21.
//
#include <tuple>

#include "Framework/TestingFramework.hpp"
#include "Utilities/MakeFromTuple.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.make_from_tuple", "[Unit][Utilities]") {
    // [make_from_tuple_eg]
    struct Foo {
      int first_;
      float second_;
      int third_;
      Foo(int first, float second, int third)
          : first_(first), second_(second), third_(third) {}
    };
    auto tuple = std::make_tuple(42, 3.14f, 0);
    auto foo = std::make_from_tuple<Foo>(std::move(tuple));
    // [make_from_tuple_eg]

    CHECK(foo.first_ == 42);
    CHECK(foo.second_ == approx(3.14f));
    CHECK(foo.third_ == 0);
  }

}  // namespace
