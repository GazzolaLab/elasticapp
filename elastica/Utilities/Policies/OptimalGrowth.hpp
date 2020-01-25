#pragma once

#include <algorithm>

namespace elastica {

  struct OptimalGrowth {
    inline size_t operator()(size_t old_size, size_t requested_size) const;
  };

  inline size_t OptimalGrowth::operator()(size_t old_size,
                                          size_t requested_size) const {
    // https://lokiastari.com/blog/2016/03/25/resizemaths/index.html
    const size_t needed(
        std::max(static_cast<size_t>(old_size * 1.6), requested_size));
    return ((needed) ? (4 * ((needed - 1) / 4 + 1)) : (0));
  }

}  // namespace elastica
