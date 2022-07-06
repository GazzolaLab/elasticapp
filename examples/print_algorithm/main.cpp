#include "Simulator/Connections.hpp"
#include "Simulator/Constraints.hpp"
#include "Simulator/Environments.hpp"
#include "Simulator/Forcing.hpp"
#include "Simulator/Simulator.hpp"
//
#include "Systems/CosseratRods.hpp"  // CosseratRod is a default system

int main(int argc, char* argv[]) {
  /*
   *
   */
  namespace ec = elastica::configuration;

  auto simulator = elastica::make_simulator<
      elastica::modules::Connections, elastica::modules::Constraints,
      elastica::modules::Environments, elastica::modules::Forcing>();
  simulator->finalize();
  simulator->print_algorithm();
}
