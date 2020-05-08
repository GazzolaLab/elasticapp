#pragma once
#include <utility>

#include "elastica/Simulator/Interface/Connections.hpp"
#include "elastica/Simulator/Interface/Constraints.hpp"
#include "elastica/Simulator/Interface/DefaultSimulatorConfig.hpp"
#include "elastica/Simulator/Interface/Simulator.hpp"
namespace elastica {
  using interface::Connections;
  using interface::Constraints;
  using interface::Simulator;
  using interface::DefaultSimulatorConfig;

  template <template <typename /*Simulator*/, typename /*SimTraits*/>
            class... AddOns,
            typename SimulationOptions_ = DefaultSimulatorConfig>
  auto make_simulator(SimulationOptions_ = {}) {
    return Simulator<SimulationOptions_, AddOns...>::instance();
//    return std::make_unique<Simulator<SimulationOptions_, AddOns...>>();
  }

}  // namespace elastica