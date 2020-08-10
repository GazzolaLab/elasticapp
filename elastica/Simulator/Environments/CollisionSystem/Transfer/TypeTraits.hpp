#pragma once

#include <type_traits>
#include <utility>

#include "elastica/Simulator/Environments/CollisionSystem/Transfer/Types.hpp"
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

      // https://stackoverflow.com/a/57132084
      // but do not need to check for signatures at this point
      template <typename PrimitiveType>
      struct IsPrimitiveDefaultTransferable<
          PrimitiveType, cpp17::void_t<decltype(&PrimitiveType::transfer())>>
          : IsRigidPrimitive<PrimitiveType> {};

      // Switch between from_t <=> to_t
      template <typename FromOrToTag>
      struct Switch {
        static constexpr tags::from_t switch_tag(tags::to_t);  // NO LINT
        static constexpr tags::to_t switch_tag(tags::from_t);
        // All other cases should return a compiler error
        static constexpr void switch_tag(...);
        using type = decltype(switch_tag(std::declval<FromOrToTag>()));
      };

    }  // namespace tt

  }  // namespace collision

}  // namespace elastica
