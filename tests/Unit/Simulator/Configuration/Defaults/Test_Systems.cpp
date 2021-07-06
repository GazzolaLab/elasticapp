//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <type_traits>

#include "Simulator/Configuration/Defaults/Systems.hpp"
#include "Simulator/Configuration/TypeTraits/Systems.hpp"

namespace {

  static_assert(
      std::is_nothrow_default_constructible<
          ::elastica::configuration::DefaultSystemsConfiguration>::value,
      "Default meta failure!");

  static_assert(
      ::elastica::tt::IsValidSystemsConfiguration<
          ::elastica::configuration::DefaultSystemsConfiguration>::value,
      "default meta failure!");

}  // namespace
