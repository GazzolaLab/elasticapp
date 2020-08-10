#pragma once

#include "elastica/Simulator/Environments/CollisionSystem/Transfer/Types.hpp"

namespace elastica {

  namespace collision {

    namespace tags {
      // Tags for transfer
      struct from_t {};
      struct to_t {};
    }  // namespace tags

    // Transfer Variables for
    constexpr tags::from_t from{};
    constexpr tags::to_t to{};

  }  // namespace collision

}  // namespace elastica
