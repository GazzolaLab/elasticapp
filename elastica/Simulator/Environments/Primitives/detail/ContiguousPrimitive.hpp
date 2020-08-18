#pragma once

#include <cmath>
#include <utility>

//////// Types first
#include "elastica/Simulator/Environments/CollisionSystem/Types.hpp"
#include "elastica/Simulator/Environments/Primitives/Types.hpp"
#include "elastica/Utilities/DefineTypes.h"
////////

#include "elastica/Simulator/Environments/Primitives/Protocols.hpp"
#include "elastica/Simulator/Environments/Primitives/TypeTraits.hpp"
#include "elastica/Utilities/ProtocolHelpers.hpp"
#include "elastica/Utilities/Requires.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {

  namespace collision {

    namespace detail {

      // This is a primitive type meant for continuous deformable
      // media like a Cosserat rod
      template <class PT,
                Requires<cpp17::conjunction_v<
                    tt::IsRigidPrimitive<PT>,
                    ::tt::conforms_to<PT, protocols::ConvexPrimitive>>> =
                    nullptr>
      class ContiguousPrimitive final : public PT {
       public:
        using PrimitiveType = PT;

        template <typename... Args>
        explicit ContiguousPrimitive(Args&&... args)
            : PrimitiveType(args...), next_(nullptr), prev_(nullptr) {}

        ContiguousPrimitive(ContiguousPrimitive&& other) noexcept
            : PrimitiveType(std::move(other)),
              next_(other.next_),
              prev_(other.prev_) {
          other.next_ = nullptr;
          other.prev_ = nullptr;
        };

        ~ContiguousPrimitive() final = default;

        PT* next() const noexcept { return next_; }
        PT* prev() const noexcept { return prev_; }

        void set_next(PT* next) noexcept { next_ = next; }
        void set_prev(PT* prev) noexcept { prev_ = prev; }

       private:
        // Store two pointers to adjacent PT
        PT *next_, *prev_;
      };

    }  // namespace detail

  }  // namespace collision

}  // namespace elastica