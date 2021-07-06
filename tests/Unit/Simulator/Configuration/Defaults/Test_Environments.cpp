//
// Created by Tejaswin Parthasarathy on 6/17/21.
//

#include <type_traits>

#include "Simulator/Configuration/Defaults/Environments.hpp"
#include "Simulator/Configuration/TypeTraits/Environments.hpp"

namespace {
  static_assert(
      std::is_nothrow_default_constructible<
          ::elastica::configuration::DefaultEnvironmentsConfiguration>::value,
      "Default meta failure!");

  static_assert(
      ::elastica::tt::IsValidEnvironmentsConfiguration<
          ::elastica::configuration::DefaultEnvironmentsConfiguration>::value,
      "default meta failure!");

}  // namespace
