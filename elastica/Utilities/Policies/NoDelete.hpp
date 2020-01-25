#pragma once

namespace elastica {
  struct NoDelete {
    template <typename T>
    inline void operator()(T& /*ptr*/) const {}
  };
}  // namespace elastica