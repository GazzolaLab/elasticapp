#pragma once

#include <type_traits>
#include <utility>

#include "elastica/Simulator/Environments/Primitives/TypeTraits.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {

    namespace tt {

      template <typename T, typename = cpp17::void_t<>>
      struct IsCollectionDefaultTransferable : std::false_type {};

      template <typename T>
      struct IsCollectionDefaultTransferable<
          T, cpp17::void_t<decltype(std::declval<T>().
                                    // get_dimensions
                                    get_dimensions(std::declval<std::size_t>())
                                    // get_dimensions
                                    ),
                           decltype(std::declval<T>().
                                    // get_position
                                    get_position(std::declval<std::size_t>())
                                    // get_position
                                    ),
                           decltype(std::declval<T>().
                                    // get_velocity
                                    get_velocity(std::declval<std::size_t>())
                                    // get_velocity
                                    ),
                           decltype(
                               std::declval<T>().
                               // get_angular_velocity
                               get_angular_velocity(std::declval<std::size_t>())
                               // get_angular_velocity
                               ),
                           decltype(std::declval<T>().
                                    // get_force
                                    get_force(std::declval<std::size_t>())
                                    // get_force
                                    ),
                           decltype(std::declval<T>().
                                    // get_torque
                                    get_torque(std::declval<std::size_t>())
                                    // get_force
                                    )>> : std::true_type {};

      template <typename PrimitiveType, typename = cpp17::void_t<>>
      struct IsPrimitiveDefaultTransferable : std::false_type {};

      template <typename PrimitiveType>
      struct IsPrimitiveDefaultTransferable<
          PrimitiveType, cpp17::void_t<decltype(&PrimitiveType::transfer())>>
          : IsRigidPrimitive<PrimitiveType> {};

    }  // namespace tt

  }  // namespace collision

}  // namespace elastica
