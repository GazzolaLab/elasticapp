#pragma once

#include <type_traits>

#include "elastica/Simulator/Domain/Types.hpp"

namespace elastica {

  // so that its easier to do and/or logic
  struct UnboundedBoundary : std::false_type {};
  struct PeriodicBoundary : std::true_type {};
}