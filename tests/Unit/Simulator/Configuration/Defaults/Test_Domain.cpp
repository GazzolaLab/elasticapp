//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include <type_traits>

#include "Simulator/Configuration/Defaults/Domain.hpp"
#include "Simulator/Configuration/TypeTraits/Domain.hpp"

namespace {

  static_assert(
      std::is_nothrow_default_constructible<
          ::elastica::configuration::DefaultDomainConfiguration>::value,
      "Default meta failure!");
  static_assert(
      ::elastica::tt::IsValidDomainConfiguration<
          ::elastica::configuration::DefaultDomainConfiguration>::value,
      "default meta failure!");

}  // namespace
