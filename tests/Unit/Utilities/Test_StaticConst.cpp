//
// Created by Tejaswin Parthasarathy on 1/21/21.
//

#include "Utilities/StaticConst.hpp"

/// [static_const_example]
template <int N>
struct GetInteger {
  constexpr int operator()() const noexcept { return N; }
};
// typical use as a global functor
constexpr auto const& get_5 = elastica::static_const<GetInteger<5>>::value;
static_assert(get_5() == 5, "What!");
/// [static_const_example]

