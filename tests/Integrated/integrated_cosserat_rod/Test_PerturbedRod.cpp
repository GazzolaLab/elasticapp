//
// Created by Seung Hyun Kim on 11/9/21.
//

#include <iostream>

#include "Framework/TestingFramework.hpp"
#include "Utilities/MakeArray.hpp"
///
#include "Systems/Block.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp" // TODO : replace later
#include "Simulator/Materials.hpp"

namespace {

  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using Plugin = elastica::cosserat_rod::CosseratRodPlugin<
      Traits, blocks::Block,
      elastica::cosserat_rod::component::WithCircularCosseratRod,
      elastica::cosserat_rod::component::
      WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;
  using Model = blocks::Block<Plugin>;

  ELASTICA_TEST_CASE("Integrated.CosseratRods.SimpleInitialization",
  "[Systems][Integrated]") {

    Model simple_rod{};

    auto initializer = elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
        // Variables
        blocks::initialize<elastica::tags::NElement>([](...) { return 5; }),
        // Geometry Variables
        blocks::initialize<elastica::tags::Position>([](...) { return 1.0; }),
        blocks::initialize<elastica::tags::Director>([](...) { return 0.5; }),
        blocks::initialize<elastica::tags::ElementDimension>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::ReferenceCurvature>(
            [](...) { return 1.0; }),
        blocks::initialize<elastica::tags::ReferenceShearStretchStrain>(
            [](...) { return 1.0; }),
        blocks::initialize<elastica::tags::ShearStretchRigidityMatrix>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::BendingTwistRigidityMatrix>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::ElementDilatation>(
            [](...) { return 1.0; }),
        blocks::initialize<elastica::tags::ForceDampingCoefficient>(
            [](...) { return 4.0; }),
        blocks::initialize<elastica::tags::TorqueDampingCoefficient>(
            [](...) { return 1.0; }),
        // Kinematics Variables
        blocks::initialize<elastica::tags::Mass>([](...) { return 1.0; }),
        blocks::initialize<elastica::tags::Velocity>([](...) { return 0.0; }),
        blocks::initialize<elastica::tags::AngularVelocity>(
            [](...) { return 0.0; }),
        blocks::initialize<elastica::tags::Acceleration>(
            [](...) { return 0.0; }),
        blocks::initialize<elastica::tags::AngularAcceleration>(
            [](...) { return 0.0; }));

    auto slicedrod = simple_rod.emplace_back(std::move(initializer));
  }

  ELASTICA_TEST_CASE("Integrated.CosseratRods.PerturbedRodTest",
  "[Systems][Integrated]") {
    Model simple_rod{};
    using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    Init straight_initializer{ // Initialization Wrapper
          Init::NElement{5UL},
          Init::Material{::elastica::iron},
          Init::Radius{0.007522},
          Init::Length{0.18},
          Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
          Init::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
          Init::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
          Init::ForceDampingCoefficient{0.2},
          Init::TorqueDampingCoefficient{0.2},
      };

    auto slicedrod = simple_rod.emplace_back(straight_initializer.initialize<Plugin>());

    slicedrod.compute_internal_forces();
    slicedrod.compute_internal_torques();


    std::cout << slicedrod.get_position().columns() << std::endl;
    std::cout << slicedrod.get_element_dimension(2UL) << std::endl;
    std::cout << slicedrod.get_second_moment_of_area(0UL) << std::endl;

    std::cout << slicedrod.get_position().columns() << std::endl;
    std::cout << slicedrod.get_element_dimension(2UL) << std::endl;
    std::cout << slicedrod.get_second_moment_of_area(0UL) << std::endl;

    //Model::initialize(simple_rod, std::move(initializer));

    std::cout << blocks::get<elastica::tags::Mass>(slicedrod) << std::endl;

    auto vol = slicedrod.get_element_volume();
    auto n_elem = (*vol).size();
    auto mass( blocks::get<::elastica::tags::Mass>(slicedrod) );
    //std::cout << Traits::DataType::Scalar::slice(mass, 0, n_elem) << std::endl;

    std::cout << slicedrod.get_element_volume() << std::endl;

  }
}  // namespace
