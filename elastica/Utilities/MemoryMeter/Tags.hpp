#pragma once

namespace elastica {

  // Forward declarations
  class MemoryMeter;

  namespace tags {

    struct MemoryMeterTag {
      using type = MemoryMeter;
    };

  }  // namespace tags

}  // namespace elastica