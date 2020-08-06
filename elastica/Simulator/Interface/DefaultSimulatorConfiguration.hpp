#pragma once

// The base simulator configuration
#include "elastica/Simulator/Interface/SimulatorConfiguration.hpp"

// The abilities
#include "elastica/Simulator/Interface/Defaults/Systems.hpp"
#include "elastica/Simulator/Interface/Defaults/Environments.hpp"


namespace elastica {

  namespace interface {


    using DefaultSimulatorConfiguration =
        SimulatorConfiguration<SystemsConfiguration,
                               EnvironmentConfiguration>;

  }  // namespace interface

}  // namespace elastica