//
// Created by Seung Hyun Kim on 11/13/21.
//

#include <iostream>

#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"

namespace ec = elastica::cosserat_rod;  // Alias

using Traits = ec::DefaultCosseratRodTraits;
using Plugin = ec::CosseratRodPlugin<
    Traits, blocks::Block, ec::WithCircularCosseratRod,
    ec::WithExplicitlyDampedDiagonalLinearHyperElasticModel,
    ec::WithRodKinematics>;
using B = blocks::Block<Plugin>;

int main() {
  /* Single rod example
   *
   */

  // Material Definition
  elastica::MaterialID myMaterial = elastica::create_material(
      7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

  // Set initializer interface
  using Init = ec::StraightCosseratRodInitializer;
  Init straight_initializer{
      // Initialization Wrapper
      Init::NElement{10UL},
      Init::Material{myMaterial},
      Init::Radius{0.007522},
      Init::Length{0.18},
      Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
      Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
      Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
      Init::ForceDampingCoefficient{0.2},
      Init::TorqueDampingCoefficient{0.2},
  };

  // Create rod
  B block{};
  auto rod = block.emplace_back(straight_initializer.initialize<Plugin>(
      // custom initialize velocity
      ::blocks::initialize<::elastica::tags::Velocity>(
          [/* custom call */](std::size_t /* index */) { return 0.1; })));

  std::cout << "Position using block::get" << std::endl;
  std::cout << blocks::get<elastica::tags::Position>(rod) << std::endl;
  std::cout << "Velocity using block::get" << std::endl;
  std::cout << blocks::get<elastica::tags::Velocity>(rod) << std::endl;

  std::cout << "Velocity using elastica::position" << std::endl;
  std::cout << ::elastica::position(rod) << std::endl;
  std::cout << "Velocity using elastica::velocity" << std::endl;
  std::cout << ::elastica::velocity(rod) << std::endl;

  return 0;
}
