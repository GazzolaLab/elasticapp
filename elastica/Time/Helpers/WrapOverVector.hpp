//
// Created by Tejaswin Parthasarathy on 6/1/21.
//

#pragma once

#include <vector>

#include "Time/SimulationTime.hpp"

namespace elastica {

  namespace timestepper {

    namespace detail {

      // adds support for vectors
      template <typename Op>
      struct WrapOverVector : Op {
        template <typename System>
        inline auto operator()(std::vector<System>& systems, Time const time_v,
                               TimeDelta const dt) const noexcept -> void {
          for (auto& system : systems)
            operator()(system, time_v, dt);
        }
      };

    }  // namespace detail

  }  // namespace timestepper

}  // namespace elastica
