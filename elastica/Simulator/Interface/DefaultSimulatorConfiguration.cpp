#include "elastica/Simulator/Interface/DefaultSimulatorConfiguration.hpp"

namespace elastica {

  namespace interface {

    // Explicitly instantiate only the classes we want to be defined.
    template struct SimulatorConfiguration<SystemsConfiguration,
                                          EnvironmentConfiguration>;

  }  // namespace interface

}  // namespace elastica
