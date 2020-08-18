#pragma once

#include <type_traits>

#include "elastica/Simulator/Environments/Primitives/RigidBodyPrimitive.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  ::elastica::collision NAMESPACE FORWARD DECLARATIONS
    //
    //==========================================================================

    //    class RigidBodyPrimitive;

    namespace tt {

      template <typename T>
      struct IsRigidPrimitive : std::is_base_of<RigidBodyPrimitive, T> {};

      template <typename T>
      constexpr bool is_rigid_primitive_v = IsRigidPrimitive<T>::value;

    }  // namespace tt

  }  // namespace collision
}  // namespace elastica
