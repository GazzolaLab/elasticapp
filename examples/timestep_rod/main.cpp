//
// Created by Seung Hyun Kim on 11/21/21.
//

#include <iostream>

#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/common/SymplecticStepperAdapter.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers/Algorithms.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"

namespace eCR = elastica::cosserat_rod;
namespace eConfig = elastica::configuration;

template <typename T, typename B>
using ComponentsWithStepper = ::elastica::SymplecticPolicy<
    T, B, eCR::component::WithCircularCosseratRod,
    eCR::component::WithExplicitlyDampedDiagonalLinearHyperElasticModel,
    eCR::component::WithRodKinematics>;

int main() {
  /* Timestepping single rod example
   *
   */

  using Traits = eCR::DefaultCosseratRodTraits;
  using Plugin =
      eCR::CosseratRodPlugin<Traits, blocks::Block, ComponentsWithStepper>;

  // Material Definition
  elastica::MaterialID myMaterial = elastica::create_material(
      7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

  // Set initializer interface
  using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
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

  // Simulator Configuration
  auto system_config =
      eConfig::make_systems_configuration<tmpl::list<Plugin>>(
          eConfig::make_blocking_policy(
              eConfig::RestrictSizeAcrossBlockTypes{50UL}),
          eConfig::make_iteration_policy(
              eConfig::iterate_across_system_types_in(eConfig::seq),
              eConfig::iterate_over_systems_in(eConfig::seq)));
  auto simulator_config = eConfig::make_simulator_configuration(
      eConfig::DefaultDomainConfiguration{}, system_config,
      eConfig::DefaultParallelConfiguration{});
  auto simulator = elastica::make_simulator<>(simulator_config);

  // Create Rod
  auto rod =
      *(simulator->emplace_back<Plugin>(straight_initializer.initialize<Plugin>(
          ::blocks::initialize<::elastica::tags::Velocity>(
              [](std::size_t) { return 0.1; }))));

  std::cout << "Position before" << std::endl;
  std::cout << ::elastica::position(rod) << std::endl;
  std::cout << "Velocity before" << std::endl;
  std::cout << ::elastica::velocity(rod) << std::endl;

  // Finalize simulator
  simulator->finalize();

  // Configure Timestepper
  using Algorithm = ::elastica::timestepper::symplectic::PositionVerlet;
  using TimeStepper =
      ::elastica::timestepper::symplectic::SymplecticStepper<Algorithm>;
  const TimeStepper time_stepper{};
  ::elastica::Time time_v{0.0};
  ::elastica::TimeDelta time_delta{0.2};

  // Timestepping
  simulator->step(time_stepper, time_v, time_delta);
  simulator->step(time_stepper, time_v, time_delta);

  std::cout << "Position after two step" << std::endl;
  std::cout << ::elastica::position(rod) << std::endl;
  std::cout << "Velocity after two step" << std::endl;
  std::cout << ::elastica::velocity(rod) << std::endl;

  return 0;
}
