//
// Created by Seung Hyun Kim on 1/2/2022
//
#include <cmath>
#include <iomanip>

#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "Simulator/Forcing.hpp"
#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"
#include "Utilities/Get.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"

int main() {
  enable_floating_point_exceptions();

  using ::elastica::cosserat_rod::CosseratRodWithoutDamping;
  using ::elastica::cosserat_rod::RodEnergy;

  // TODO : upon refactoring the above plugins in a elastica header, these
  // should be no longer needed and hence removed.
  namespace ec = elastica::configuration;
  auto system_config =
      ec::make_systems_configuration<tmpl::list<CosseratRodWithoutDamping>>(
          ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{10UL}),
          ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                    ec::iterate_over_systems_in(ec::seq)));

  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config,
      ec::DefaultParallelConfiguration{}, ec::DefaultForcingConfiguration{},
      ec::DefaultConstraintsConfiguration{});

  auto simulator = elastica::make_simulator<>(simulator_config);

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
      1e4,
      // Shear modulus
      1e4 / 1.5,
      // The below parameters doesn't change the result of this example
      // coefficient of static friction
      100.0,  // needs to be > 0.0
      // cofficient of dynamic friction
      20.0,  // needs to be > 0.0
      // Contact stiffness
      200,
      // normal contact damping
      1e-10,
      // tangent contact damping
      1e-10);

  const std::size_t n_elem{4UL};
  const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
  const ::elastica::real_t base_length{3.0};
  const ::elastica::real_t dl = base_length / n_elem;
  const std::size_t half_n_elem = n_elem / 2UL;

  const ::elastica::Vec3 horizontal_direction({0.0, 0.0, 1.0});
  const ::elastica::Vec3 vertical_direction({1.0, 0.0, 0.0});
  const ::elastica::real_t angle_of_inclination = M_PI / 4;

  // clang-format off
  auto position_profile =
      [origin, dl, angle_of_inclination, horizontal_direction, vertical_direction](std::size_t index) -> ::elastica::Vec3 {
    if (index < half_n_elem + 1)
      return (origin +
              dl * static_cast<::elastica::real_t>(index) *
                  (std::cos(angle_of_inclination) * horizontal_direction +
                   std::sin(angle_of_inclination) * vertical_direction));
    else
      return (origin +
              dl * static_cast<::elastica::real_t>(half_n_elem) *
                  (std::cos(angle_of_inclination) * horizontal_direction +
                   std::sin(angle_of_inclination) * vertical_direction)) +
             (dl * static_cast<::elastica::real_t>(index - half_n_elem) *
              (std::cos(angle_of_inclination) * horizontal_direction -
               std::sin(angle_of_inclination) * vertical_direction));
  };

  auto director_profile =
      [angle_of_inclination, horizontal_direction, vertical_direction]
      (std::size_t index) -> ::elastica::Rot3 {
    if (index < half_n_elem) {
      return ::elastica::Rot3(
        {{ 0.0                           , 1.0, 0.0                           },
         {-std::sin(angle_of_inclination), 0.0, std::cos(angle_of_inclination)},
         { std::cos(angle_of_inclination), 0.0, std::sin(angle_of_inclination)}}
      );
    } else {
      return ::elastica::Rot3(
        {{ 0.0                           , 1.0, 0.0                           },
         {-std::sin(angle_of_inclination), 0.0,-std::cos(angle_of_inclination)},
         {-std::cos(angle_of_inclination), 0.0, std::sin(angle_of_inclination)}}
      );
    }};
  // clang-format on

  using Init =
      ::elastica::cosserat_rod::UserDefinedCosseratRodWithoutDampingInitializer<
          decltype(position_profile), decltype(director_profile)>;

  Init initializer{
      // Initialization Wrapper
      Init::NElement{n_elem},
      Init::Material{my_material},
      Init::Radius{0.25},
      Init::Position(std::move(position_profile)),
      Init::Director(std::move(director_profile)),
  };

  auto rod_it = simulator->emplace_back<CosseratRodWithoutDamping>(
      initializer.initialize<CosseratRodWithoutDamping>());

  simulator->finalize();

  // simulator->print_algorithm();

  const ::elastica::real_t final_time = 40.0;
  const ::elastica::real_t dt = 0.01 * dl;
  const auto total_steps = static_cast<std::size_t>(final_time / dt);

  using TimeStepper = ::elastica::timestepper::PositionVerlet;
  const TimeStepper time_stepper{};

  ::elastica::Time time_v{0.0};
  ::elastica::TimeDelta time_delta{dt};

  /* Pre processing */
  for (std::size_t i_step = {0UL}; i_step < total_steps; ++i_step) {
    simulator->step(time_stepper, time_v, time_delta);
    if (not(i_step % 100)) {
      // Intermediate processes (if needed)
      // clang-format off
      std::cout << std::setprecision(5)
                << "Trans.Energy = "
                << RodEnergy<>::compute_translational_energy(*rod_it)
                << ' '
                << "Rot.Energy = "
                << RodEnergy<>::compute_rotational_energy((*rod_it))
                << ' '
                << "Shear.Energy = "
                << RodEnergy<>::compute_shear_energy(*rod_it)
                << ' '
                << "Bend.Energy = "
                << RodEnergy<>::compute_bend_energy(*rod_it)
                << ' '
                << '\n';
      // clang-format on
    }
  }

  /* Post processing */
}