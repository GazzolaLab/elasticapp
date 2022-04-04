//==============================================================================
/*!
//  \file
//  \brief Simulation reset example
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#include <elastica/Simulator/Connections.hpp>
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/EnableReset.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/IgnoreUnused.hpp>

int main() {
  using ::elastica::cosserat_rod::CosseratRod;

  namespace ec = elastica::configuration;
  auto system_config = ec::make_systems_configuration<tmpl::list<CosseratRod>>(
      ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{10UL}),
      ec::DefaultIterationPolicy{});
  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config,
      ec::DefaultParallelConfiguration{}, ec::DefaultForcingConfiguration{},
      ec::DefaultConstraintsConfiguration{},
      ec::DefaultConnectionsConfiguration{});

  auto simulator = ::elastica::make_simulator<
      ::elastica::modules::Forcing, ::elastica::modules::Constraints,
      ::elastica::modules::Connections,
      // Also add a reset module. This gives access to a reset() method to the
      // simulator which you can use to reset it to an empty state.
      ::elastica::modules::EnableReset>(simulator_config);

  ::elastica::Time time_v{0.0};

  // in case simulator is already ready, reset it first.
  // This usually occurs when the simulator is pre-made (for e.g. in a
  // case-study where it is stored as a class member and hence already created)
  if (simulator->ready())
    simulator->reset(time_v);  // time_v is optional, default is 0.0

  // Now you are free to add rods, connections and constraints
  elastica::MaterialID myMaterial = elastica::create_material(
      7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

  using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
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

  // Add two systems
  auto first_rod_ref = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());
  auto second_rod_ref = simulator->emplace_back<CosseratRod>(
      straight_initializer
          .but_with(Init::Origin{::elastica::Vec3{1.0, 0.0, 0.0}})
          .initialize<CosseratRod>());

  namespace eco = ::elastica::constraints;

  auto empty_constraint = [](auto /*time*/, auto& /*rod_system*/,
                             auto /*index*/) {};
  auto constraint_generator =
      eco::make_constraint(eco::make_value_constraint(empty_constraint),
                           eco::make_rate_constraint(empty_constraint));
  // Add one constraint
  simulator->constrain(first_rod_ref).at(0UL).with(constraint_generator());

  namespace efo = ::elastica::forcing;

  auto empty_force = [](auto /*time*/, auto& /*rod_system*/, auto /*index*/) {};
  auto force_generator = efo::make_forcing(efo::make_force(empty_force),
                                           efo::make_torque(empty_force));
  // Add one force
  simulator->force(first_rod_ref)
      .at(::elastica::end - 1UL)
      .with(force_generator());

  namespace econ = ::elastica::connections;

  auto empty_connection = [](auto /*time*/, auto& /*first*/,
                             auto /*first_index*/, auto& /*second*/,
                             auto /*second_index*/) {};
  auto connection_generator = econ::make_connection(
      econ::make_force(empty_connection), econ::make_torque(empty_connection));

  using ::elastica::interface::at;
  // Add one connectoin
  simulator->connect(first_rod_ref, at(::elastica::end - 1UL))
      .to(second_rod_ref, at(0UL))
      .with(connection_generator());

  // Now finalize the simulation
  simulator->finalize();

  // We step once in time,
  namespace et = elastica::timestepper;
  using TimeStepper = ::elastica::timestepper::PositionVerlet;
  const TimeStepper time_stepper{};
  ::elastica::TimeDelta time_delta{0.002};

  simulator->step(time_stepper, time_v, time_delta);

  /*
   * Usually, after finalize(), adding systems/connections are disallowed. But
   * with the reset method(), you can undo the finalization. This also undos the
   * systems/connections/constraints/forces added.
   *
   * This is useful in multi-processing with a fixed number of \elastica
   * simulator processes (such as convergence studies, inverse-design, RL)
   */
  simulator->reset();

  // now add more rods and see no errors!
  auto another_rod_ref = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());
  auto yet_another_rod_ref = simulator->emplace_back<CosseratRod>(
      straight_initializer
          .but_with(Init::Origin{::elastica::Vec3{0.0, 1.0, 0.0}})
          .initialize<CosseratRod>());
  elastica::ignore_unused(another_rod_ref, yet_another_rod_ref);
}
