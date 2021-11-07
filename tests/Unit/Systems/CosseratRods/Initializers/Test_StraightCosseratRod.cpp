//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp"

namespace {
  using namespace elastica::cosserat_rod;
}  // namespace

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.StraightCosseratRod",
      "[Systems][Unit]") {
    // using T = typename StraightCosseratRod::NElement;
    (void)(StraightCosseratRod::NElement(20UL));
  }

}  // namespace
