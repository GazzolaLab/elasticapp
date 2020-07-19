#pragma once

#include <cstdint>
#include <vector>

namespace elastica {

  class Material;
  using Materials = std::vector<Material>;
  using MaterialID = uint32_t;

}  // namespace elastica
