//
// Created by Seung Hyun Kim on 11/13/21.
//

#include <iostream>

#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/common/SymplecticStepperAdapter.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers/Algorithms.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"

template <typename T, typename B>
using ComponentsWithStepper = ::elastica::SymplecticPolicy<
    T, B, elastica::cosserat_rod::component::WithCircularCosseratRod,
    elastica::cosserat_rod::component::
        WithExplicitlyDampedDiagonalLinearHyperElasticModel,
    elastica::cosserat_rod::component::WithRodKinematics>;

using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
using Plugin = elastica::cosserat_rod::CosseratRodPlugin<Traits, blocks::Block,
                                                         ComponentsWithStepper>;
using Model = blocks::Block<Plugin>;

using real_type = typename Traits::DataType::real_type;

int main() {
  namespace ec = elastica::configuration;

  elastica::MaterialID myMaterial = elastica::create_material(
      7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

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

  auto system_config = ec::make_systems_configuration<tmpl::list<Plugin>>(
      ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
      ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                ec::iterate_over_systems_in(ec::seq)));

  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config,
      ec::DefaultParallelConfiguration{});

  auto simulator = elastica::make_simulator<>(simulator_config);

  auto rod =
      simulator->emplace_back<Plugin>(straight_initializer.initialize<Plugin>(
          ::blocks::initialize<::elastica::tags::Velocity>(
              [](std::size_t) { return 0.1; })));
  // auto second_rod = simulator->emplace_back<Plugin>(
  //     straight_initializer.initialize<Plugin>());

  std::cout << ::elastica::position(rod) << std::endl;

  simulator->finalize();

  using Algorithm = ::elastica::timestepper::symplectic::PositionVerlet;
  using TimeStepper =
      ::elastica::timestepper::symplectic::SymplecticStepper<Algorithm>;
  const TimeStepper time_stepper{};

  ::elastica::Time time_v{0.0};
  ::elastica::TimeDelta time_delta{0.2};
  simulator->step(time_stepper, time_v, time_delta);
  simulator->step(time_stepper, time_v, time_delta);

  std::cout << ::elastica::position(rod) << std::endl;

  return 0;
}
