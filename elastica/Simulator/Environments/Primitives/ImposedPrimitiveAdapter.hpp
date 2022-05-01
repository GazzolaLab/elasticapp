//
// Created by Tejaswin Parthasarathy on 3/24/22.
//

#pragma once

#include "Simulator/Environments/Primitives/Types.hpp"

namespace elastica {

  namespace collision {

    // T is a elastica Primitive conforming to protocols::ConvexPrimitive
    template <typename Primitive_>
    class ImposedPrimitiveAdapter final : public Primitive_ {
     public:
      using Parent = Primitive_;
      using Parent::Parent;
    };

  }  // namespace collision

}  // namespace elastica