//
// Created by Tejaswin Parthasarathy on 1/20/22.
//

#pragma once

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      enum class InvRotateDivideKind { scalar, simd, blaze };

      template <InvRotateDivideKind OpKind>
      struct InvRotateDivideOp;

    }  // namespace detail

  }  // namespace cosserat_rod

}  // namespace elastica
