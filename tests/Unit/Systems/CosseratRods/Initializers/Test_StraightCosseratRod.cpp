//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {
  using namespace elastica::cosserat_rod;
  using ::elastica::Vec3;
}  // namespace

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Initializers.StraightCosseratRodInitializer",
      "[Systems][Unit]") {
    // check if all structs are accessible
    (void)(StraightCosseratRodInitializer::NElement(20UL));

    // [straight_cosserat_rod_eg]
    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Init straight_initializer{
        Init::NElement{20UL},
        Init::Material{::elastica::iron},
        Init::Radius{0.2},
        Init::Length{1.0},
        Init::Origin{Vec3{0.0, 0.0, 0.0}},
        Init::Direction{Vec3{0.0, 0.0, 1.0}},
        Init::Normal{Vec3{0.0, 1.0, 0.0}},
        Init::ForceDampingCoefficient{0.2},
        Init::TorqueDampingCoefficient{0.2}
    };
    // using Plugin = /* */;
    // straight_initializer.initializer<Plugin>();
    // [straight_cosserat_rod_eg]
  }

}  // namespace
