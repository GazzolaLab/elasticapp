//
// Created by Tejaswin Parthasarathy on 11/20/21.
//

// define to use TBB backend (for now)
#define ELASTICA_USE_TBB 1
#include <tbb/global_control.h>

#include "Simulator/Materials.hpp"
#include "Simulator/Materials/Materials.hpp"
#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers.hpp"

int main(int argc, char* argv[]) {
  /*
   *
   */

  using ::elastica::cosserat_rod::CosseratRod;

  const int n_threads_used_by_application = 4;
  // controls the application level parallelism
  tbb::global_control application_limit(
      tbb::global_control::max_allowed_parallelism,
      n_threads_used_by_application);

  namespace ec = elastica::configuration;

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

  auto system_config = ec::make_systems_configuration<tmpl::list<Plugin>>(
      ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{10UL}),
      ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                // use parallelism for a singlr type of system
                                ec::iterate_over_systems_in(ec::par)));
  auto parallel_config =
      ec::make_parallel_configuration(n_threads_used_by_application);

  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config, parallel_config);

  auto simulator = elastica::make_simulator<>(simulator_config);

  // these will be iterated over in parallel.
  for (std::size_t i_rod = 0UL; i_rod < 20UL; ++i_rod) {
    simulator->emplace_back<Plugin>(straight_initializer.initialize<Plugin>());
  }

  simulator->finalize();

  namespace et = elastica::timestepper;
  using TimeStepper = ::elastica::timestepper::PositionVerlet;
  const TimeStepper time_stepper{};

  ::elastica::Time time_v{0.0};
  ::elastica::TimeDelta time_delta{0.002};

  simulator->step(time_stepper, time_v, time_delta);
}
