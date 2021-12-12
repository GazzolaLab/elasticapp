//
// Created by Tejaswin Parthasarathy on 11/20/21.
//

// define to use TBB backend (for now)
#define ELASTICA_USE_TBB 1

#include "Simulator/Materials.hpp"
#include "Simulator/Materials/Materials.hpp"
#include "Simulator/Simulator.hpp"
#include "Systems/Block.hpp"
#include "Systems/Block/Protocols.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/Initializers/StraightCosseratRod.hpp"  // TODO : replace later
#include "Systems/common/SymplecticStepperAdapter.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/SymplecticSteppers/Algorithms.hpp"
#include "Time/SymplecticSteppers/SymplecticStepper.hpp"
#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

namespace {

  template <typename T, typename B>
  using ComponentsWithStepper = ::elastica::SymplecticPolicy<
      T, B, elastica::cosserat_rod::component::WithCircularCosseratRod,
      elastica::cosserat_rod::component::
          WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;

  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using Plugin =
      elastica::cosserat_rod::CosseratRodPlugin<Traits, blocks::Block,
                                                ComponentsWithStepper>;
  using Model = blocks::Block<Plugin>;

  using real_type = typename Traits::DataType::real_type;

  // auto approx(Catch::Detail::Approx(0).epsilon(1e-8));

}  // namespace

int main(int argc, char* argv[]) {
  /*
   *
   */

  namespace ec = elastica::configuration;
  namespace et = elastica::timestepper;

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

  auto simulator_config = ec::make_simulator_configuration(
      ec::DefaultDomainConfiguration{}, system_config,
      ec::DefaultParallelConfiguration{});

  auto simulator = elastica::make_simulator<>(simulator_config);

  // these will be iterated over in parallel.
  for (std::size_t i_rod = 0UL; i_rod < 20UL; ++i_rod) {
    simulator->emplace_back<Plugin>(straight_initializer.initialize<Plugin>());
  }

  simulator->finalize();

  using Algorithm = et::symplectic::PositionVerlet;
  using TimeStepper = et::symplectic::SymplecticStepper<Algorithm>;
  const TimeStepper time_stepper{};

  ::elastica::Time time_v{0.0};
  ::elastica::TimeDelta time_delta{0.002};

  simulator->step(time_stepper, time_v, time_delta);
}
