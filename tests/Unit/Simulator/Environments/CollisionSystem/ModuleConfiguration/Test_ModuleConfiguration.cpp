//
// Created by Tejaswin Parthasarathy on 4/19/22.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/ModuleConfiguration/ModuleConfiguration.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Environments.CollisionSystem.make_collision_configuration",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    using namespace elastica::configuration;
    auto a = make_collision_configuration(seq, seq);
    (void)make_collision_configuration(seq, par);
    (void)make_collision_configuration(par, seq);
    (void)make_collision_configuration(par, par);

    static_assert(cpp17::is_same_v<typename decltype(a)::ForceModel,
                                   ELASTICA_DEFAULT_DEM_RESPONSE_MODEL>,
                  "Meta-failure");
  }
}  // namespace
