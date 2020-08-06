#pragma once

namespace elastica {

  // TODO : general is_power functions

  // From https://stackoverflow.com/a/19399478
  template <typename IntType, typename std::enable_if_t<
      std::is_integral<IntType>::value, int> = 0>
  constexpr bool is_power_of_2(IntType v) { return v && ((v & (v - 1)) == 0); }

}  // namespace elastica