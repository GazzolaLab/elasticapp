//
// Created by Tejaswin Parthasarathy on 12/27/21.
//
#include <iomanip>

#include "Simulator/Constraints.hpp"
#include "Simulator/Forcing.hpp"
#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/common/SymplecticStepperAdapter.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers/Algorithms.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"
#include "Utilities/Get.hpp"
#include "Utilities/StdHelpers.hpp"

// TODO : refactor away into an elastica header
template <typename T, typename B>
using ComponentsWithStepper = ::elastica::SymplecticPolicy<
    T, B, elastica::cosserat_rod::component::WithCircularCosseratRod,
    elastica::cosserat_rod::component::
        WithExplicitlyDampedDiagonalLinearHyperElasticModel,
    elastica::cosserat_rod::component::WithRodKinematics>;

using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
using CosseratRod =
    elastica::cosserat_rod::CosseratRodPlugin<Traits, blocks::Block,
                                              ComponentsWithStepper>;
using B = blocks::Block<CosseratRod>;

using real_type = typename Traits::DataType::real_type;

int main() {
  // TODO : upon refactoring the above plugins in a elastica header, these
  // should be no longer needed and hence removed.
  namespace ec = elastica::configuration;
  auto system_config = ec::make_systems_configuration<tmpl::list<CosseratRod>>(
      ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
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
      // material density
      1e3,  // pyelastica values
      // coefficient of restitution
      0.24,
      // Young's modulus
      1e4,
      // Shear modulus
      1e4,  // doesn't matter
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
  const ::elastica::real_t base_length{1.0};
  const std::size_t n_elem{19UL};

  StraightInit straight_initializer{
      // StraightInitialization Wrapper
      StraightInit::NElement{n_elem},
      StraightInit::Material{my_material},
      StraightInit::Radius{0.025},
      StraightInit::Length{base_length},
      StraightInit::Origin{origin},
      StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
      StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
      StraightInit::ForceDampingCoefficient{2.0},
      StraightInit::TorqueDampingCoefficient{2.0},
  };
  //

  auto rod_it = simulator->emplace_back<CosseratRod>(
      straight_initializer.initialize<CosseratRod>());

  using namespace ::elastica::constraints;
  const std::size_t my_constraint_index = 0UL;
  auto constraint_generator = make_constraint(
      make_value_constraint([origin, origin_frame = ::elastica::director(
                                         *rod_it, my_constraint_index)](
                                auto /*time*/, auto& rod_system, auto index) {
        ::elastica::position(rod_system, index) = origin;
        ::elastica::director(rod_system, index) = origin_frame;
      }),
      make_rate_constraint(
          [](auto /*time*/, auto& rod_system, std::size_t index) {
            ::elastica::velocity(rod_system, index) = 0.0;
            ::elastica::angular_velocity(rod_system, index) = 0.0;
          }));

  simulator->constrain(*rod_it).at(0UL).with(constraint_generator());

  using namespace ::elastica::forcing;

  const ::elastica::Vec3 F{1.0, 0.0, 0.0};
  auto force_generator = make_forcing(
      make_force([F](auto /*time*/, auto& rod_system, std::size_t index) {
        using Tag = ::elastica::tags::ExternalLoads;
        // using V =
        // cpp17::remove_reference_t<decltype(rod_system)>::VariableMap
        blaze::column(::elastica::get<Tag>(rod_system), index) = F;
      }),
      make_torque([](auto /*time*/, auto&, std::size_t) {}));
  simulator->force(*rod_it).at(::elastica::end - 1UL).with(force_generator());

  simulator->finalize();

  // simulator->print_algorithm();

  const ::elastica::real_t final_time = 20.0;
  const ::elastica::real_t dl = base_length / n_elem;
  const ::elastica::real_t dt = 0.01 * dl;
  const auto total_steps = static_cast<std::size_t>(final_time / dt);

  using Algorithm = ::elastica::timestepper::symplectic::PositionVerlet;
  using TimeStepper =
      ::elastica::timestepper::symplectic::SymplecticStepper<Algorithm>;
  const TimeStepper time_stepper{};

  ::elastica::Time time_v{0.0};
  ::elastica::TimeDelta time_delta{dt};

  std::vector<::elastica::real_t> times;
  std::vector<::elastica::real_t> tip_displacements;
  times.reserve(400UL);
  tip_displacements.reserve(400UL);

  for (std::size_t i_step = {0UL}; i_step < total_steps; ++i_step) {
    // is it really right
    simulator->step(time_stepper, time_v, time_delta);

    if (not(i_step % 100)) {
      // whatever
      tip_displacements.push_back(::elastica::position(*rod_it, n_elem)[0UL]);
      times.push_back(static_cast<::elastica::real_t>(time_v));
      // do some processing
    }
  }

  // std::cout << times << std::endl;

  // std::cout << static_cast<::elastica::real_t>(time_v) << std::endl;
  // std::cout << ::elastica::position(*rod_it) << std::endl;
  // std::cout << ::elastica::velocity(*rod_it) << std::endl;

  const auto n_samples = tip_displacements.size();

  for (std::size_t i_sample = {0UL}; i_sample < n_samples; ++i_sample) {
    std::cout << std::setprecision(16) << times[i_sample] << ","
              << tip_displacements[i_sample] << std::endl;
  }
}