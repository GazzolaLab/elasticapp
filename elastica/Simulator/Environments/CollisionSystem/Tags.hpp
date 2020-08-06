#pragma once

#include <string>

#include "Configuration.hpp"
#include "elastica/Utilities/Timing.h"
#include "elastica/Utilities/MemoryMeter.hpp"

namespace elastica {

  namespace collision {

    // Forward declare collision system
    template <class SimulatorConfig,
              class CollisionConfig>  // Type of the configuration
    class CollisionSystem;

    namespace tags {

      // TODO : Type trait adherence to have a type member
      // Usage
      struct CollisionDetectionTag {
        template <typename SimulatorConfig>
        using type =
            CollisionSystem<SimulatorConfig, collision::CollisionConfig>;
      };

      // These are for internal use and is not exposed in environment tags

      struct CollisionDetectionElapsedTimeTag : timing::tags::WcTimerTag {
        // TODO
        //    Annoying fix because our current Tagged Tuple implementation
        //    needs data_type instead of type
        using data_type = typename timing::tags::WcTimerTag::type;
        static std::string name() noexcept { return "Collision Detection Time"; }
      };

      struct CollisionResponseElapsedTimeTag : timing::tags::WcTimerTag {
        using data_type = typename timing::tags::WcTimerTag::type;
        static std::string name() noexcept { return "Collision Response Time "; }
      };

      struct TransferIntoElapsedTimeTag : timing::tags::WcTimerTag {
        using data_type = typename timing::tags::WcTimerTag::type;
        static std::string name() noexcept { return "Transfer into CD Time   "; }
      };

      struct TransferOutOfElapsedTimeTag : timing::tags::WcTimerTag {
        using data_type = typename timing::tags::WcTimerTag::type;
        static std::string name() noexcept { return "Transfer out of CD Time "; }
      };

      struct TotalElapsedTimeTag : timing::tags::WcTimerTag {
        using data_type = typename timing::tags::WcTimerTag::type;
        static std::string name() noexcept { return "Total time for CD       "; }
      };

      struct CollisionDetectionMemoryTag : ::elastica::tags::MemoryMeterTag {
        // TODO
        //    Annoying fix because our current Tagged Tuple implementation
        //    needs data_type instead of type
        using data_type = typename ::elastica::tags::MemoryMeterTag::type;
        static std::string name() noexcept { return "Collision Detection Mem."; }
      };

      struct CollisionResponseMemoryTag : ::elastica::tags::MemoryMeterTag{
        // TODO
        //    Annoying fix because our current Tagged Tuple implementation
        //    needs data_type instead of type
        using data_type = typename ::elastica::tags::MemoryMeterTag::type;
        static std::string name() noexcept { return "Collision Response Mem. "; }
      };

    }  // namespace tags

  }  // namespace collision

}  // namespace elastica