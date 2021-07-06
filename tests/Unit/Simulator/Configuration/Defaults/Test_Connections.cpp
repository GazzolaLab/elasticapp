//
// Created by Tejaswin Parthasarathy on 6/17/21.
//

#include <type_traits>

#include "Simulator/Configuration/Defaults/Connections.hpp"
#include "Simulator/Configuration/TypeTraits/Connections.hpp"

namespace {

  static_assert(
      std::is_nothrow_default_constructible<
          ::elastica::configuration::DefaultConnectionsConfiguration>::value,
      "Default meta failure!");
  static_assert(
      ::elastica::tt::IsValidConnectionsConfiguration<
          ::elastica::configuration::DefaultConnectionsConfiguration>::value,
      "default meta failure!");

}  // namespace