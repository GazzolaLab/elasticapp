//
// Created by Tejaswin Parthasarathy on 2/18/22.
//

#include <tuple>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/GravityField/GravityField.hpp"
#include "Simulator/Interface/Environments.hpp"
#include "Simulator/Materials.hpp"
#include "Simulator/Simulator.hpp"
#include "Systems/CosseratRods.hpp"
#include "Time/SimulationTime.hpp"
//
#include "Utilities/DefineTypes.h"
#include "Utilities/Get.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

// namespace mock{
//
//   struct Traits{
//     using TimeType = ::elastica::Time;
//   };
//
//   struct Simulator{
//
//     using type = std::tuple<int, float, char>;
//   };
// }

// not advisable, but okay since Simulator is already tested
using CosseratRod = ::elastica::cosserat_rod::CosseratRod;

struct Configuration {
  static inline auto make_simulator_configuration() {
    namespace ec = elastica::configuration;
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRod>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    return ec::make_simulator_configuration(ec::DefaultDomainConfiguration{},
                                            system_config,
                                            ec::DefaultParallelConfiguration{});
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<>(make_simulator_configuration());
  }
};

namespace {
  using namespace ::elastica::environments;

  using G =
      GravityField<decltype(Configuration::make_simulator_configuration())>;

  ELASTICA_TEST_CASE("Unit.Environments.GravityField", "[Unit][Environments]") {
    static_assert(::tt::assert_conforms_to<
                      G, ::elastica::environments::protocols::Environment>,
                  "Meta-failure");

    auto g = G::instance();
    auto simulator = Configuration::make_simulator();

    (void)(g->initialize(*simulator));
    ::elastica::Vec3 my_gravity{0.2, 0.2, 0.3};
    g->gravity() = my_gravity;
    CHECK(cpp17::as_const(g)->gravity() == my_gravity);

    ::elastica::Time time_v{0.0};
    using ::elastica::environments::reset;
    reset(*g, time_v);
    CHECK_FALSE(cpp17::as_const(g)->gravity() == my_gravity);

    using ::elastica::configuration::make_gravity_configuration;
    using ::elastica::environments::initialize_parameters;
    auto config = make_gravity_configuration(my_gravity);
    initialize_parameters(*g, config);
    CHECK(cpp17::as_const(g)->gravity() == my_gravity);
  }

  ELASTICA_TEST_CASE("Unit.Environments.GravityField.resolve",
                     "[Unit][Environments]") {
    auto g = G::instance();
    auto simulator = Configuration::make_simulator();

    ::elastica::Vec3 my_gravity{0.1, 0.0, 0.0};
    g->gravity() = my_gravity;

    elastica::MaterialID my_material = elastica::create_material(
        // material density
        1e3,  // pyelastica values
        // coefficient of restitution
        0.24,
        // Young's modulus
        1e5,
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

    const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
    const ::elastica::real_t base_length = 0.1;
    const std::size_t n_elem = 20UL;

    using StraightInit =
        ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    StraightInit straight_initializer{
        // StraightInitialization Wrapper
        StraightInit::NElement{n_elem},
        StraightInit::Material{my_material},
        StraightInit::Radius{0.002},
        StraightInit::Length{base_length},
        StraightInit::Origin{origin},
        StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
        StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
        StraightInit::ForceDampingRate{0.1},
        StraightInit::TorqueDampingRate{0.1},
    };

    auto rod_ref = simulator->emplace_back<CosseratRod>(
        straight_initializer.initialize<CosseratRod>());

    // Check for external tags
    auto to_vec = [](auto&& blaze_struct) -> std::vector<::elastica::real_t> {
      return std::vector<::elastica::real_t>(
          blaze_struct.data(),
          blaze_struct.data() + blaze::capacity(blaze_struct));
    };

    auto actual =
        to_vec(::elastica::get<::elastica::tags::ExternalLoads>(rod_ref.get()));
    auto expected = std::vector<::elastica::real_t>(actual.size(), 0.0);

    CHECK_ITERABLE_APPROX(actual, expected);

    ::elastica::Time time_v{0.0};
    g->resolve(*simulator, time_v);

    actual =
        to_vec(::elastica::get<::elastica::tags::ExternalLoads>(rod_ref.get()));
    CHECK_FALSE(actual == expected);
  }

}  // namespace

namespace {

  struct IntegratedConfiguration {
    static_assert(::tt::assert_conforms_to<
                      ::elastica::environments::tags::GravityField,
                      ::elastica::environments::protocols::EnvironmentTag>,
                  "Meta-failure");

    static inline auto make_simulator_configuration() {
      namespace ec = elastica::configuration;
      auto system_config =
          ec::make_systems_configuration<tmpl::list<CosseratRod>>(
              ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
              ec::make_iteration_policy(
                  ec::iterate_across_system_types_in(ec::seq),
                  ec::iterate_over_systems_in(ec::seq)));
      // tested elsewhere
      auto gravity_config = ec::make_gravity_configuration(0.1, 0.0, 0.0);
      auto env_config = ec::make_environments_configuration<
          tmpl::list<::elastica::environments::tags::GravityField>>(
          ec::seq, gravity_config);

      return ec::make_simulator_configuration(
          ec::DefaultDomainConfiguration{}, system_config,
          ec::DefaultParallelConfiguration{}, env_config);
    }

    static inline auto make_simulator() {
      return elastica::make_simulator<::elastica::modules::Environments>(
          make_simulator_configuration());
    }
  };

  ELASTICA_TEST_CASE("Unit.z.Integrated.Simulator.Environments.GravityField",
                     "[Unit][Simulator][Environments]") {
    auto simulator = IntegratedConfiguration::make_simulator();
    ::elastica::Vec3 my_gravity{0.1, 0.0, 0.0};

    auto& gravity_environment =
        simulator
            ->get_environment<::elastica::environments::tags::GravityField>();
    gravity_environment->gravity() = my_gravity;
    //    simulator->set_gravity(my_gravity);

    // Copy-pasted from non-integrated test earlier
    elastica::MaterialID my_material = elastica::create_material(
        // material density
        1e3,  // pyelastica values
        // coefficient of restitution
        0.24,
        // Young's modulus
        1e5,
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

    const ::elastica::Vec3 origin{0.0, 0.0, 0.0};
    const ::elastica::real_t base_length = 0.1;
    const std::size_t n_elem = 20UL;

    using StraightInit =
        ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    StraightInit straight_initializer{
        // StraightInitialization Wrapper
        StraightInit::NElement{n_elem},
        StraightInit::Material{my_material},
        StraightInit::Radius{0.002},
        StraightInit::Length{base_length},
        StraightInit::Origin{origin},
        StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
        StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
        StraightInit::ForceDampingRate{0.1},
        StraightInit::TorqueDampingRate{0.1},
    };

    auto rod_ref = simulator->emplace_back<CosseratRod>(
        straight_initializer.initialize<CosseratRod>());

    // Check for external tags
    auto to_vec = [](auto&& blaze_struct) -> std::vector<::elastica::real_t> {
      return std::vector<::elastica::real_t>(
          blaze_struct.data(),
          blaze_struct.data() + blaze::capacity(blaze_struct));
    };

    auto actual =
        to_vec(::elastica::get<::elastica::tags::ExternalLoads>(rod_ref.get()));
    auto expected = std::vector<::elastica::real_t>(actual.size(), 0.0);

    CHECK_ITERABLE_APPROX(actual, expected);

    ::elastica::Time time_v{0.0};
    simulator->apply_environmental_effects(time_v);

    actual =
        to_vec(::elastica::get<::elastica::tags::ExternalLoads>(rod_ref.get()));
    CHECK_FALSE(actual == expected);
  }

}  // namespace