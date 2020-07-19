#pragma once

#include "Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"

namespace elastica {

  namespace collision {

    namespace response {

      // TBB Style operator for serial calculations
      struct LinearSpringDashpot {
        inline LinearSpringDashpot() = default;
        ~LinearSpringDashpot() = default;

        // Prevents implicit conversions
        //        template <class U>
        //        real_t operator()(U) const noexcept= delete;
        //        template <>
        //        real_t operator()<ContactID>(ContactID c) const noexcept;

        // In case the template isnt specified
        real_t operator()(ContactID c) const noexcept;
        //        real_t operator()(int c) const noexcept;
      };

    }  // namespace response

  }  // namespace collision

}  // namespace elastica
