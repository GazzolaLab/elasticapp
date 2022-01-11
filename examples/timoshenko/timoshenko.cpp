//
// Created by Seung Hyun Kim on 1/2/2022
//
#include <chrono>

#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "Simulator/Constraints.hpp"
#include "Simulator/Forcing.hpp"
#include "Simulator/Materials.hpp"
#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers.hpp"
#include "Utilities/Get.hpp"

int main() {
  enable_floating_point_exceptions();

  using ::elastica::cosserat_rod::CosseratRod;

  // TODO : Add CosseratRod to ELASTICA_DEFAULT_ADMISSIBLE_PLUGINS_FOR_SYSTEMS
  namespace ec = elastica::configuration;
  auto system_config = ec::make_systems_configuration<tmpl::list<CosseratRod>>(
      ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{100UL}),
      ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                ec::iterate_over_systems_in(ec::seq)));

  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config,
      ec::DefaultParallelConfiguration{}, ec::DefaultForcingConfiguration{},
      ec::DefaultConstraintsConfiguration{});

  auto simulator = elastica::make_simulator<::elastica::modules::Forcing,
                                            ::elastica::modules::Constraints>(
      simulator_config);

  /*
   * 2. Rod Initialization
   * We then construct a Cosserat rod using an initializer. In this tutorial
   * we dont focus on the initializers. To know more about them, please look at
   * `tutorial_rod_construction`.
   */
  elastica::MaterialID my_material = elastica::create_material(
      // PyElastica values
      // material density
      5e3,
      // coefficient of restitution
      0.24,
      // Young's modulus
      1e6,
      // Shear modulus
      1e4,
      // The below parameters doesn't change the result of this example
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10,
      // coefficient of static friction
      0.1,
      // cofficient of dynamic friction
      0.1);

  using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;

  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{3.0};
  const std::size_t n_elem{100UL};

  StraightInit straight_initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{n_elem},
      StraightInit::Material{my_material},
      StraightInit::Radius{0.25},
      StraightInit::Length{base_length},
      StraightInit::Origin{origin},
      StraightInit::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
      StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
      StraightInit::ForceDampingCoefficient{2.0},
      StraightInit::TorqueDampingCoefficient{2.0},
  };
  //

  auto rod_it = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());

  namespace eco = ::elastica::constraints;
  const std::size_t my_constraint_index = 0UL;
  auto constraint_generator = eco::make_constraint(
      eco::make_value_constraint(
          [origin,
           origin_frame = ::elastica::director(*rod_it, my_constraint_index)](
              auto /*time*/, auto& rod_system, auto index) {
            ::elastica::position(rod_system, index) = origin;
            ::elastica::director(rod_system, index) = origin_frame;
          }),
      eco::make_rate_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system, index) = 0.0;
            ::elastica::angular_velocity(rod_system, index) = 0.0;
          }));

  simulator->constrain(*rod_it).at(0UL).with(constraint_generator());

  namespace efo = ::elastica::forcing;

  const ::elastica::Vec3 F{-15.0, 0.0, 0.0};
  auto force_generator = efo::make_forcing(
      efo::make_force([F](auto /*time*/, auto& rod_system, std::size_t index) {
        using Tag = ::elastica::tags::ExternalLoads;
        // using V =
        // cpp17::remove_reference_t<decltype(rod_system)>::VariableMap
        blaze::column(::elastica::get<Tag>(rod_system), index + 1) = F;
      }),
      efo::make_torque([](auto /*time*/, auto&, std::size_t) {}));
  simulator->force(*rod_it).at(::elastica::end - 1UL).with(force_generator());

  simulator->finalize();

  // simulator->print_algorithm();

  const ::elastica::real_t final_time = 5000;
  const ::elastica::real_t dl = base_length / n_elem;
  const ::elastica::real_t dt = 0.01 * dl;
  const auto total_steps = static_cast<std::size_t>(final_time / dt);

  using TimeStepper = ::elastica::timestepper::PositionVerlet;
  const TimeStepper time_stepper{};

  ::elastica::Time time_v{0.0};
  ::elastica::TimeDelta time_delta{dt};

  /* Pre processing */

  for (std::size_t i_step = {0UL}; i_step < total_steps; ++i_step) {
    simulator->step(time_stepper, time_v, time_delta);
  }

  /* Post processing */
}