#pragma once

#include <cstddef>

#include "elastica/ErrorHandling/Assert.hpp"

namespace elastica {

  template <typename Real>
  struct Span {
    static constexpr std::size_t min_idx = 0UL;
    static constexpr std::size_t max_idx = 1UL;

    explicit Span(Real arg_min, Real arg_max) {
      // TODO : No assert in constructor
      ELASTICA_ASSERT(
          arg_min < arg_max,
          "Wrong span of simulation, please provide min first and max second!");
      span_[min_idx] = arg_min;
      span_[max_idx] = arg_max;
    };
    ~Span() = default;
    Span(const Span& other) = default;
    Span(Span&& other) noexcept = default;

    Real span_[2UL];
  };
}  // namespace elastica