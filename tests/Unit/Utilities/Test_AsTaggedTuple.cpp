//
// Created by Tejaswin Parthasarathy on 6/21/21.
//

#include <tuple>

#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace {
  /// [as_tagged_tuple_eg]
  struct A {
    using type = int;
  };
  struct B {
    using type = float;
  };
  struct C {
    using type = char;
  };

  using correct_type = tuples::TaggedTuple<A, B, C>;

  using type_list = tmpl::list<A, B, C>;
  using first_ = tmpl::as_tagged_tuple<type_list>;
  static_assert(std::is_same_v<first_, correct_type>,
                "AsTaggedTuple meta-failure!");

  using tuple_list = std::tuple<A, B, C>;
  using second_ = tmpl::as_tagged_tuple<tuple_list>;
  static_assert(std::is_same_v<second_, correct_type>,
                "AsTaggedTuple meta-failure!");

  using tagged_tuple_list = tuples::TaggedTuple<A, B, C>;
  using third_ = tmpl::as_tagged_tuple<tagged_tuple_list>;
  static_assert(std::is_same_v<third_, correct_type>,
                "AsTaggedTuple meta-failure!");
  /// [as_tagged_tuple_eg]

}  // namespace