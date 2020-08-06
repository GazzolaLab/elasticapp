/*=============================================================================
  Copyright (c) 2011-2019 Bolero MURAKAMI
  https://github.com/bolero-MURAKAMI/Sprout
  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once

#include <limits>
#include <type_traits>

#include "elastica/Utilities/ValueTraits/Configuration.hpp"

namespace elastica {

  template <typename FloatType,
            typename std::enable_if_t<std::is_floating_point<FloatType>::value,
                                      int> = 0>
  inline constexpr bool is_inf(FloatType x) {
    return
#if ELASTICA_USE_BUILTIN_CMATH_FUNCTION
        __builtin_isinf(x)
#else
        x == std::numeric_limits<FloatType>::infinity() or
        x == -std::numeric_limits<FloatType>::infinity()
#endif
            ;
  }

  template <typename IntType, typename std::enable_if_t<
                                  std::is_integral<IntType>::value, int> = 0>
  inline constexpr bool is_inf(Int x) {
    return false;
  }

}  // namespace elastica
