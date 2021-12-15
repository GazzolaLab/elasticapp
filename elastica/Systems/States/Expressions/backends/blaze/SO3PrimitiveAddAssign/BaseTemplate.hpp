//
// Created by Tejaswin Parthasarathy on 12/1/21.
//

#pragma once

namespace elastica {

  namespace states {

    namespace detail {

      enum class SO3AddAssignKind { scalar, simd };

      template <SO3AddAssignKind Op>
      struct SO3AddAssign;

    }  // namespace detail

  }  // namespace states

}  // namespace elastica