//
// Created by Tejaswin Parthasarathy on 5/25/21.
//

#pragma once

#include "Utilities/DefineTypes.h"

namespace elastica {

  namespace detail {

    template <typename /* Real */>
    class SimulationTime;

    template <typename /* Real */>
    class SimulationTimeDelta;

  }  // namespace detail

  using Time = detail::SimulationTime<real_t>;
  using TimeDelta = detail::SimulationTimeDelta<real_t>;

}  // namespace elastica