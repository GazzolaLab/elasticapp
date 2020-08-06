#pragma once

#include <cstddef>

namespace elastica{

  template <typename Real>
  struct Span {
    static constexpr std::size_t min_idx = 0UL;
    static constexpr std::size_t max_idx = 1UL;

    explicit Span(Real arg_min, Real arg_max) {
      span_[min_idx] = arg_min;
      span_[max_idx] = arg_max;
    };
    ~Span() = default;
    Span(const Span& other) = default;
    Span(Span&& other) = default;

    Real span_[2UL];
  };
}