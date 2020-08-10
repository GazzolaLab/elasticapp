#pragma once
#include <utility>

#include "Simulator/Interface/DefaultSimulatorConfiguration.hpp"
#include "elastica/Simulator/Interface/Connections.hpp"
#include "elastica/Simulator/Interface/Constraints.hpp"
#include "elastica/Simulator/Interface/Environments.hpp"
#include "elastica/Simulator/Interface/Simulator.hpp"

namespace elastica {
  using interface::DefaultSimulatorConfiguration;
  using interface::Connections;
  using interface::Constraints;
  using interface::Environments;
  using interface::Simulator;

  template <template <typename /*Simulator*/, typename /*SimTraits*/>
            class... AddOns,
            typename SimulationOptions_ = DefaultSimulatorConfiguration>
  auto make_simulator(SimulationOptions_ = {}) {
    return Simulator<SimulationOptions_, AddOns...>::instance();
    //    return std::make_unique<Simulator<SimulationOptions_, AddOns...>>();
  }

}  // namespace elastica