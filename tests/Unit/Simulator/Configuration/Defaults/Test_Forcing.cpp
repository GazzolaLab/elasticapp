//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <type_traits>

#include "Simulator/Configuration/Defaults/Forcing.hpp"
#include "Simulator/Configuration/TypeTraits/Forcing.hpp"

namespace {
  static_assert(
      std::is_nothrow_default_constructible<
          ::elastica::configuration::DefaultForcingConfiguration>::value,
      "Default meta failure!");

  static_assert(
      ::elastica::tt::IsValidForcingConfiguration<
          ::elastica::configuration::DefaultForcingConfiguration>::value,
      "default meta failure!");

}
