//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <type_traits>

#include "Simulator/Configuration/DefaultSimulatorConfiguration.hpp"
#include "Simulator/Configuration/TypeTraits.hpp"

namespace {

  static_assert(
      std::is_nothrow_default_constructible<
          ::elastica::configuration::DefaultSimulatorConfiguration>::value,
      "Default meta failure!");

  static_assert(
      ::elastica::tt::IsValidEnvironmentsConfiguration<
          ::elastica::configuration::DefaultSimulatorConfiguration>::value,
      "default meta failure!");

  static_assert(
      ::elastica::tt::IsValidConstraintsConfiguration<
          ::elastica::configuration::DefaultSimulatorConfiguration>::value,
      "meta failure!");

  static_assert(
      ::elastica::tt::IsValidForcingConfiguration<
          ::elastica::configuration::DefaultSimulatorConfiguration>::value,
      "meta failure!");

  static_assert(
      ::elastica::tt::IsValidConnectionsConfiguration<
          ::elastica::configuration::DefaultSimulatorConfiguration>::value,
      "meta failure!");

  static_assert(
      ::elastica::tt::IsValidSystemsConfiguration<
          ::elastica::configuration::DefaultSimulatorConfiguration>::value,
      "meta failure!");

  static_assert(
      ::elastica::tt::IsValidDomainConfiguration<
          ::elastica::configuration::DefaultSimulatorConfiguration>::value,
      "meta failure!");

}  // namespace