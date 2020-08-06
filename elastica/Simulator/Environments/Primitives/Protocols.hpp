#pragma once

#include <utility>
#include <type_traits>

#include "elastica/Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace protocols {

      /*!
       * \brief Has support functions.
       *
       * Requires the class has these member functions:
       * - `support`: Returns the maximum point as a `Vec3` in the search
       * direction as a `Vec3`
       * - `support_contact_threshold`: Returns the maximum point as a `Vec3` in
       * the search direction as a `Vec3` but with some contact tolerance
       */
      struct ConvexPrimitive {
        template <typename ConformingType>
        struct test {
          // Try calling the `ConformingType::support` member function
          using support_return_type = decltype(
              std::declval<ConformingType>().support(std::declval<Vec3>()));

          // Try calling the `ConformingType::support_contact_threshold` member
          // function
          using support_contact_threshold_return_type = decltype(
              std::declval<ConformingType>().support(std::declval<Vec3>()));

          // Check the return type of the `support` member function
          static_assert(
              std::is_same<support_return_type, Vec3>::value,
              "The 'support' function must return a 'elastica::Vec3'.");

          // Check the return type of the `support_contact_threshold` member
          // function
          static_assert(
              std::is_same<support_contact_threshold_return_type, Vec3>::value,
              "The 'support_contact_threshold' function must return a "
              "'elastica::Vec3'.");
        };
      };

    }  // namespace protocols

  }  // namespace collision

}  // namespace elastica