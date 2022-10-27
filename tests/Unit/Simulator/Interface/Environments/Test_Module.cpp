//
// Created by Tejaswin Parthasarathy on 2/18/22.
//
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
/// mocks
#include "Simulator/Interface/Environments/MockEnvironments.hpp"
#include "Simulator/Interface/MockSimulator.hpp"
#include "Simulator/Interface/MockSystems.hpp"
#include "Simulator/Interface/Systems/Mocks.hpp"  // for iteration policy and what not
/// serializatoin
#include "Simulator/Configuration.hpp"
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/MockSimulatorSerialize.hpp"
#include "Utilities/Singleton.hpp"

namespace {

  enum FieldStatus { initialized = 0, finalized, resolved, resetted, unwanted };
  template <typename Traits>
  struct MockField : public elastica::Singleton<MockField<Traits>>
  /*: public ::tt::ConformsTo<::elastica::protocols::Environments>*/ {
   public:
    template <typename Simulator>
    void resolve(Simulator const&, typename Traits::TimeType) {
      status = resolved;
    }
    template <typename Simulation>
    void initialize(Simulation const&) {
      status = finalized;
    }
    FieldStatus status = unwanted;
  };

  struct WithMockFieldConfiguration {
    auto status() const { return status_; }
    FieldStatus status_ = initialized;
  };

  template <typename Traits>
  void reset(MockField<Traits>& field,
             typename Traits::TimeType /* meta */) noexcept {
    field.status = FieldStatus::resetted;
  }

  // templated because
  template <typename Traits, typename Config>
  void initialize_parameters(MockField<Traits>& field,
                             Config const& config) noexcept {
    field.status = config.status();
  }

  struct MockFieldTag {
    using requires_primitives = std::false_type;

    template <typename Traits>
    using type = MockField<Traits>;
  };

  // so that code compiles
  struct WithPrimitivesConfiguration {
    // using ListOfAdmissibleSystemCollections = tmpl::list<>;
  };

  auto get_systems_configuration() {
    using namespace ::elastica::configuration;
    return mock::make_systems_configuration<tmpl::list<>>(
        DefaultBlockingPolicy{},
        make_iteration_policy(iterate_across_system_types_in(seq),
                              // for testing iteration policy
                              iterate_over_systems_in(seq)));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Environments.Module",
      "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    SECTION("Empty tags") {
      EMPTY_TEST();

      using EnvironmentTags = tmpl::list<>;
      auto env_config =
          mock::make_environments_configuration_with_tags<EnvironmentTags>(
              ::elastica::configuration::seq);

      auto sim_config(
          mock::make_simulator_configuration(env_config,
                                             // needed for iteration policy
                                             get_systems_configuration(),
                                             // needed for with_primitives
                                             WithPrimitivesConfiguration{}));
      // check cases without an error first.
      bool arg_all_registered = true;
      bool is_final = false;
      auto sim = mock::make_simulator<::mock::EnvironmentsModule>(
          sim_config, arg_all_registered, is_final);
      elastica::ignore_unused(sim);

      static_assert(
          ::tt::assert_conforms_to<
              decltype(sim), ::elastica::interface::protocols::Environments>,
          "Meta failure");
      static_assert(sim.n_environments() == 0UL, "meta failure");
    }
    SECTION("Non-empty tags") {
      using EnvironmentTags = tmpl::list<MockFieldTag>;

      auto env_config =
          mock::make_environments_configuration_with_tags<EnvironmentTags>(
              ::elastica::configuration::seq);

      auto sim_config(mock::make_simulator_configuration(
          env_config,
          // needed for iteration policy
          get_systems_configuration(), WithPrimitivesConfiguration{},
          WithMockFieldConfiguration{}));
      // check cases without an error first.
      bool arg_all_registered = true;
      bool is_final = false;
      auto sim = mock::make_simulator<::mock::EnvironmentsModule>(
          sim_config, arg_all_registered, is_final);
      elastica::ignore_unused(sim);

      static_assert(
          ::tt::assert_conforms_to<
              decltype(sim), ::elastica::interface::protocols::Environments>,
          "Meta failure");
      static_assert(sim.n_environments() == 1UL, "meta failure");

      (void)(sim.environments());
      (void)(cpp17::as_const(sim).environments());

      auto& mock_field = sim.get_environment<MockFieldTag>();
      // it initializes to unwated, but then initialze_params shuold set it
      // right
      CHECK(mock_field->status == FieldStatus::initialized);

      // explictly set it
      mock_field->status = FieldStatus::finalized;

      auto const& cmock_field =
          cpp17::as_const(sim).get_environment<MockFieldTag>();
      CHECK(cmock_field->status == FieldStatus::finalized);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Environments.Module.finalize",
      "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    SECTION("Empty tags") {
      EMPTY_TEST();

      using EnvironmentTags = tmpl::list<>;
      auto env_config =
          mock::make_environments_configuration_with_tags<EnvironmentTags>(
              ::elastica::configuration::seq);
      auto sim_config(mock::make_simulator_configuration(
          env_config,
          // needed for iteration policy
          get_systems_configuration(), WithPrimitivesConfiguration{}));

      // check cases without an error first.
      bool arg_all_registered = true;
      bool is_final = false;
      auto sim = mock::make_simulator<::mock::EnvironmentsModule>(
          sim_config, arg_all_registered, is_final);

      sim.finalize();
    }

    SECTION("Non-empty tags") {
      using EnvironmentTags = tmpl::list<MockFieldTag>;
      auto env_config =
          mock::make_environments_configuration_with_tags<EnvironmentTags>(
              ::elastica::configuration::seq);
      auto sim_config(mock::make_simulator_configuration(
          env_config,
          // needed for iteration policy
          get_systems_configuration(), WithPrimitivesConfiguration{},
          WithMockFieldConfiguration{}));

      // check cases without an error first.
      bool arg_all_registered = true;
      bool is_final = false;
      auto sim = mock::make_simulator<::mock::EnvironmentsModule>(
          sim_config, arg_all_registered, is_final);

      // we cannot check what it is, since its a singleton and hence depends on
      // which test is executed first. But we can check what its not
      CHECK_FALSE(std::get<0UL>(sim.environments())->status ==
                  FieldStatus::finalized);

      sim.finalize();

      CHECK(std::get<0UL>(sim.environments())->status ==
            FieldStatus::finalized);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Environments.Module.timers",
      "[Unit][Simulator][Interface][Environments]") {
    // clang-format on

    using EnvironmentTags = tmpl::list<MockFieldTag>;
    auto env_config =
        mock::make_environments_configuration_with_tags<EnvironmentTags>(
            ::elastica::configuration::seq);
    auto sim_config(mock::make_simulator_configuration(
        env_config,
        // needed for iteration policy
        get_systems_configuration(), WithPrimitivesConfiguration{},
        WithMockFieldConfiguration{}));

    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::EnvironmentsModule>(
        sim_config, arg_all_registered, is_final);
    sim.finalize();

    (void)(sim.environment_timers());
    (void)(cpp17::as_const(sim).environment_timers());
    CHECK(cpp17::as_const(sim).get_environment_timer<MockFieldTag>().counter() == 0UL);
    sim.get_environment_timer<MockFieldTag>().start();
    sim.get_environment_timer<MockFieldTag>().stop();
    CHECK(sim.get_environment_timer<MockFieldTag>().counter() == 1UL);
    sim.get_environment_timer<MockFieldTag>().get_counter() = 0UL; // mutate
    CHECK(cpp17::as_const(sim).get_environment_timer<MockFieldTag>().counter() == 0UL);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Environments.Module.apply_environmental_forces",
      "[Unit][Simulator][Interface][Environments]") {
    // clang-format on

    using EnvironmentTags = tmpl::list<MockFieldTag>;
    auto env_config =
        mock::make_environments_configuration_with_tags<EnvironmentTags>(
            ::elastica::configuration::seq);
    auto sim_config(mock::make_simulator_configuration(
        env_config,
        // needed for iteration policy
        get_systems_configuration(), WithPrimitivesConfiguration{},
        WithMockFieldConfiguration{}));

    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::EnvironmentsModule>(
        sim_config, arg_all_registered, is_final);
    sim.finalize();

    // we cannot check what it is, since its a singleton and hence depends on
    // which test is executed first. But we can check what its not
    CHECK_FALSE(std::get<0UL>(sim.environments())->status ==
                FieldStatus::resolved);

    ::elastica::Time time_v{0.0};
    sim.apply_environmental_effects(time_v);

    CHECK(std::get<0UL>(sim.environments())->status == FieldStatus::resolved);
  }

  // clang-format off
    ELASTICA_TEST_CASE(
        "Unit.Simulator.Interface.Environments.Module.reset",
        "[Unit][Simulator][Interface][Environments]") {
    // clang-format on

    using EnvironmentTags = tmpl::list<MockFieldTag>;
    auto env_config =
        mock::make_environments_configuration_with_tags<EnvironmentTags>(
            ::elastica::configuration::seq);
    auto sim_config(mock::make_simulator_configuration(
        env_config,
        // needed for iteration policy
        get_systems_configuration(), WithPrimitivesConfiguration{},
        WithMockFieldConfiguration{}));

    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::EnvironmentsModule>(
        sim_config, arg_all_registered, is_final);
    sim.finalize();

    // we cannot check what it is, since its a singleton and hence depends on
    // which test is executed first. But we can check what its not
    CHECK_FALSE(std::get<0UL>(sim.environments())->status ==
                FieldStatus::resetted);
    sim.get_environment_timer<MockFieldTag>().start();
    sim.get_environment_timer<MockFieldTag>().stop();
    CHECK(sim.get_environment_timer<MockFieldTag>().counter() == 1UL);

    ::elastica::Time time_v{0.0};
    sim.reset_module(time_v);

    CHECK(std::get<0UL>(sim.environments())->status == FieldStatus::resetted);
    CHECK(sim.get_environment_timer<MockFieldTag>().counter() == 0UL);
  }

}  // namespace
