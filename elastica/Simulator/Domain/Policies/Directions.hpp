#pragma once

#include <cstddef>

#include "Utilities/NonCreatable.hpp"

namespace elastica {

  namespace detail {

    struct Directions : public NonCreatable {
      using DirectionType = unsigned char;
      static constexpr DirectionType X = 0;
      static constexpr DirectionType Y = 1;
      static constexpr DirectionType Z = 2;
    };

  }  // namespace detail

}  // namespace elastica
