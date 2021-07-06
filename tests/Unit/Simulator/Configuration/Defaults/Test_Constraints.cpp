//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <type_traits>

#include "Simulator/Configuration/Defaults/Constraints.hpp"
#include "Simulator/Configuration/TypeTraits/Constraints.hpp"

namespace {

  static_assert(
      std::is_nothrow_default_constructible<
          ::elastica::configuration::DefaultConstraintsConfiguration>::value,
      "Default meta failure!");
  static_assert(
      ::elastica::tt::IsValidConstraintsConfiguration<
          ::elastica::configuration::DefaultConstraintsConfiguration>::value,
      "default meta failure!");

}  // namespace
