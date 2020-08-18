#pragma once

#include "Simulator/Environments/Primitives/detail/ContiguousPrimitive.hpp"
#include "elastica/Simulator/Environments/Primitives/CapsulePrimitive.hpp"

namespace elastica {

  namespace collision {

    using ContiguousCapsulePrimitive = detail::ContiguousPrimitive<CapsulePrimitive>;

  }  // namespace collision

}  // namespace elastica
