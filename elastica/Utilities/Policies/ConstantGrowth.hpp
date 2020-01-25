#pragma once

#include <algorithm>

namespace elastica {

  template <size_t Growth>
  struct ConstantGrowth {
    inline size_t operator()(size_t old_size, size_t requested_size) const;
  };

  // Specialization for illegal Growth of 0
  template <> struct ConstantGrowth<0>;

  template <size_t Growth>
  inline size_t ConstantGrowth<Growth>::operator()(
      size_t old_size, size_t requested_size) const {
    const size_t needed(std::max(old_size + Growth, requested_size));
    return ((needed) ? (4 * ((needed - 1) / 4 + 1)) : (0));
  }

}  // namespace elastica
