#include <blaze/math/Matrix.h>

#pragma once

namespace elastica {

  template <typename... Args>
  inline auto is_identity(Args&&... args) -> bool {
    return blaze::isIdentity(std::forward<Args>(args)...);
  }

}  // namespace elastica