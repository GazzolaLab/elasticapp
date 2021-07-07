//
// Created by Tejaswin Parthasarathy on 5/25/21.
//

#include "Time/TypeTraits.hpp"
#include "Time/Types.hpp"

namespace {

  /// [is_timedelta_eg]
  using delta_type = elastica::TimeDelta;
  static_assert(elastica::tt::IsTimeDelta<delta_type>::value,
                "It is a time delta type!");
  static_assert(not elastica::tt::IsTimeDelta<int>::value,
                "It is not a time delta type!");
  /// [is_timedelta_eg]

}  // namespace