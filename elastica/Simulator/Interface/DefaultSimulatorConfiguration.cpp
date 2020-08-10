#include "elastica/Simulator/Interface/DefaultSimulatorConfiguration.hpp"

namespace elastica {

  namespace interface {

    // Explicitly instantiate only the classes we want to be defined.
    // https://stackoverflow.com/questions/2351148/explicit-template-instantiation-when-is-it-used
    // Sad but true :
    // https://stackoverflow.com/questions/11705407/explicit-template-instantiation-through-a-typedef
    template struct SimulatorConfiguration<defaults::DomainConfiguration,
                                           defaults::SystemsConfiguration,
                                           defaults::EnvironmentConfiguration>;

  }  // namespace interface

}  // namespace elastica
