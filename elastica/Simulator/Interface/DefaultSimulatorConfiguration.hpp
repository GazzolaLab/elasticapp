#pragma once

// The base simulator configuration
#include "elastica/Simulator/Interface/SimulatorConfiguration.hpp"

// The abilities
#include "elastica/ModuleSettings/Domain.hpp"
#include "elastica/ModuleSettings/Environments.hpp"
#include "elastica/ModuleSettings/Systems.hpp"

namespace elastica {

  namespace interface {

    using DefaultSimulatorConfiguration =
        SimulatorConfiguration<defaults::DomainConfiguration,
                               defaults::SystemsConfiguration,
                               defaults::EnvironmentConfiguration>;

  }  // namespace interface

}  // namespace elastica