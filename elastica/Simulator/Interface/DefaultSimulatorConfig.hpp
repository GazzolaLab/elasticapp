#pragma once

namespace elastica {
  namespace interface {
    // DONE : Config class for simulator and all its Interface classes
    struct DefaultSimulatorConfig {
      using SizeType = std::size_t;  // size of data-structures
      using IndexType = SizeType;    // indexing rods
      using TimeType = double;
      template <typename T>
      using SystemInterfaceType = std::shared_ptr<T>;
    };

  }  // namespace interface
}  // namespace elastica