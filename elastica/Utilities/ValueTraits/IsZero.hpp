/*=============================================================================
  Copyright (c) 2011-2019 Bolero MURAKAMI
  https://github.com/bolero-MURAKAMI/Sprout
  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#pragma once

#include "elastica/Utilities/ValueTraits/Configuration.hpp"

namespace elastica {

  template <typename ValueType>
  inline constexpr bool is_zero(ValueType x) {
    return (x == ValueType(0));
  }

}  // namespace elastica
