//
// Created by Tejaswin Parthasarathy on 10/13/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/Parallel.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Parallel.make_parallel_configuration",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using namespace elastica::configuration;

    /// [make_parallel_eg]
    auto parallel_configuration = make_parallel_configuration(8UL);
    /// [make_parallel_eg]

    CHECK(parallel_configuration.n_threads() == 8UL);

    // user is asshole
    auto another_parallel_configuration = make_parallel_configuration(-2);

    // get from environment, may also be dynamic
    CHECK(another_parallel_configuration.n_threads() > 0UL);
  }

}  // namespace