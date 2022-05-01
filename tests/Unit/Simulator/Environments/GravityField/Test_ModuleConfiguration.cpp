//
// Created by Tejaswin Parthasarathy on 4/30/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Environments/GravityField/ModuleConfiguration/ModuleConfiguration.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Environments.GravityField.make_gravity_configuration",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    using namespace elastica::configuration;
    auto a = make_gravity_configuration(0.0, 2.0, 3.0);
    CHECK(a.gravity() == ::elastica::Vec3{0.0, 2.0, 3.0});

    auto b = make_gravity_configuration(::elastica::Vec3{0.0, 2.0, 3.0});
    CHECK(b.gravity() == ::elastica::Vec3{0.0, 2.0, 3.0});
  }
}  // namespace
