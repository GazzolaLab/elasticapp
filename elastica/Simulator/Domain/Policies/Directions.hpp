#pragma once

#include <cstddef>

#include "Utilities/NonCreatable.hpp"

namespace elastica {

  namespace detail {

    struct Directions  : public NonCreatable {
      static constexpr unsigned char X = 0;
      static constexpr unsigned char Y = 1;
      static constexpr unsigned char Z = 2;
    };

  }  // namespace detail

}  // namespace elastica
