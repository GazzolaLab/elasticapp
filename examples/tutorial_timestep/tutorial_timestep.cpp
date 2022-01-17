//
// Created by Seung Hyun Kim on 11/21/21.
//

#include <iostream>

#include "Framework/TestingFramework.hpp"
#include "Utilities/MakeArray.hpp"
///
#include "Systems/Block.hpp"
#include "Systems/Block/Protocols.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp" // TODO : replace later
#include "Simulator/Materials.hpp"
#include "Simulator/Materials/Materials.hpp"

#include "blaze/blaze.h"
#include "blaze_tensor/blaze.h"

namespace {

  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using Plugin = elastica::cosserat_rod::CosseratRodPlugin<
      Traits, blocks::Block,
      elastica::cosserat_rod::component::WithCircularCosseratRod,
      elastica::cosserat_rod::component::
          WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;
  using Model = blocks::Block<Plugin>;

  using real_type = typename Traits::DataType::real_type;

    /*
     * rng_seed = 100
     *  Test initialization step without any additional perturbation
     */
    elastica::MaterialID myMaterial = elastica::create_material(
        7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);
    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Model simple_rod{};
    Init straight_initializer{
        // Initialization Wrapper
        Init::NElement{5UL},
        Init::Material{myMaterial},
        Init::Radius{0.007522},
        Init::Length{0.18},
        Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
        Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
        Init::ForceDampingCoefficient{0.2},
        Init::TorqueDampingCoefficient{0.2},
    };
    auto slicedrod =
        simple_rod.emplace_back(straight_initializer.initialize<Plugin>());

  }

}  // namespace
