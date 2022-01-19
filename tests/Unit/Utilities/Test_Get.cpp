//
// Created by Tejaswin Parthasarathy on 12/13/21.
//
#include <tuple>
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Utilities/Get.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.Get.get", "[Unit][Utilities]") {
    /// [get_tuple_example]
    auto a = std::tuple<int, float, char>{1, 2.1F, 'c'};
    // instead of doing std::get, one can do elastica::get
    CHECK(::elastica::get<int>(a) == 1);
    CHECK(::elastica::get<char>(a) == 'c');
    /// [get_tuple_example]

    /// [get_pair_example]
    auto b = std::pair<int, char>{2, 'd'};
    // instead of doing std::get, one can do elastica::get
    CHECK(::elastica::get<int>(b) == 2);
    CHECK(::elastica::get<char>(b) == 'd');
    /// [get_pair_example]

    /// [get_tagged_tuple_example]
    struct FirstTag {
      using type = int;
    };
    struct SecondTag {
      using type = char;
    };
    auto c = tuples::TaggedTuple<FirstTag, SecondTag>{3, 'e'};
    // elastica::get also works with other custom types such as a tagged tuple
    CHECK(::elastica::get<FirstTag>(c) == 3);
    CHECK(::elastica::get<SecondTag>(c) == 'e');
    /// [get_tagged_tuple_example]
  }

}  // namespace
