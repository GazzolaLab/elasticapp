//
// Created by Tejaswin Parthasarathy on 6/20/21.
//

#include <memory>
#include <thread>

#include "Configuration/Profiling.hpp"
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration.hpp"
#include "Simulator/Interface/MockComponent.hpp"
#include "Simulator/Interface/Simulator.hpp"  // all of simulator
#include "Simulator/Simulator.hpp"            // all of simulator
#include "Utilities/AsConst.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/WidthStream.hpp"

// confirm if friendships work
//
namespace {
  using namespace elastica;
  using namespace elastica::interface;
  using namespace elastica::configuration;
}  // namespace

namespace {

  template <typename SimTraits,  // Configuration parameter
            template <typename /*SimTraits*/, typename /*Simulator*/>
            class... Modules>  // Additional modules
  class TestSimulatorInterface : public Simulator<SimTraits, Modules...> {
    using S = Simulator<SimTraits, Modules...>;

   public:
    using typename S::AlgorithmSteps;
    using typename S::ApplyOverSystems;

    using S::algorithms;  // tested
    using S::apply_post_dynamic_updates;
    using S::apply_post_kinematic_updates;
    using S::apply_pre_dynamic_updates;
    using S::apply_updates;
    using S::batching_algorithms;  // tested
    using S::check_domain;         // tested
    using S::check_if_final;       // tested
    using S::check_if_not_final;   // tested
    using S::domain;               // tested
    using S::finalize;             // indirecly tested, since it aborts
    using S::finalize_modules;     // tested
    /// implicitly tested
    using S::get_algorithm;           // tested
    using S::get_batching_algorithm;  // tested
    using S::get_timer;               // tested
    ///
    using S::info_post_dynamic_updates;    // tested
    using S::info_post_kinematic_updates;  // tested
    using S::info_pre_dynamic_updates;     // tested
    using S::info_update;                  // tested
    using S::is_final;                     // tested
    using S::ready;                        // tested
    using S::S;                            // tested
    using S::stream_profiling_summary_to;

    using S::reset_modules;
  };

  template <template <typename /*Simulator*/,  // base simulator
                      typename /*SimTraits*/>  // simulator traits
            class... AddOns,                   // all capabilities
            typename SimulationConfig_ =       // configuration
            configuration::DefaultSimulatorConfiguration>
  auto& make_test_simulator(SimulationConfig_ config = {}) {
    using TSI = TestSimulatorInterface<SimulationConfig_, AddOns...>;
    return *std::static_pointer_cast<TSI>(TSI::initialize(std::move(config)));
  }

}  // namespace

namespace {

  auto get_periodic_domain_config() {
    return ::elastica::configuration::make_domain_configuration(
        elastica::boundary_along<elastica::EulerianFrame::X,
                                 elastica::IsPeriodic>(-5.0, 5.0),
        elastica::boundary_along<elastica::EulerianFrame::Y,
                                 elastica::IsPeriodic>(-50.0, 50.0),
        elastica::boundary_along<elastica::EulerianFrame::Z,
                                 elastica::IsPeriodic>(-5.0, 5.0));
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.buffers",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<>();
    (void)(cpp17::as_const(simulator).domain());

    (void)(simulator.algorithms());
    (void)(cpp17::as_const(simulator).algorithms());

    (void)(simulator.batching_algorithms());
    (void)(cpp17::as_const(simulator).batching_algorithms());
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.check_domain",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<>();
    CHECK_NOTHROW(simulator.check_domain());

    auto periodic_config = make_simulator_configuration(
        get_periodic_domain_config(), DefaultSystemsConfiguration{},
        DefaultParallelConfiguration{});
    auto& another_simulator = make_test_simulator(periodic_config);
    CHECK_NOTHROW(another_simulator.check_domain());
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.check_if_final",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<>();
    CHECK_NOTHROW(simulator.check_if_not_final());
    // treat it as a black box
    simulator.finalize();
    CHECK_THROWS_WITH(simulator.check_if_not_final(),
                      Catch::Contains("has been finalized"));
    CHECK(simulator.is_final());
    CHECK(simulator.ready());

    // need to make it with a different configuration to not make the statics
    // trigger
    auto periodic_config = make_simulator_configuration(
        get_periodic_domain_config(), DefaultSystemsConfiguration{},
        DefaultParallelConfiguration{});
    auto& another_simulator = make_test_simulator(periodic_config);
    CHECK_THROWS_WITH(another_simulator.check_if_final(),
                      Catch::Contains("not yet been finalized"));

    another_simulator.finalize();
    CHECK_NOTHROW(simulator.check_if_final());
  }

  // empty module with some random functionality
  template <class SimTraits, class Sim>
  struct TestAlgorithms {
    struct ConnectionFillerBatchAlgorithm {
      ConnectionFillerBatchAlgorithm(SimTraits const&) {}
    };
    using BatchingAlgorithm =
        tmpl::list<tmpl::type_<ConnectionFillerBatchAlgorithm>>;
    struct X {
      X(std::tuple<Sim*, SimTraits const&>) {}
      inline auto operator()(::elastica::Time) const noexcept -> void {
        // do some processing by waiting and biding time.
        // this is to test the timers
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5ms);
      };
    };

    struct FirstPostKinematicUpdateStep {
      using type = X;
      //**Info method*******************************************************
      static std::string info(Sim const&) noexcept {
        return "--Apply first post kinematic update\n";
      }
      //********************************************************************
    };
    struct SecondPostKinematicUpdateStep {
      using type = X;
      //**Info method*******************************************************
      static std::string info(Sim const&) noexcept {
        return "--Apply second post kinematic update\n";
      }
      //********************************************************************
    };
    struct PreDynamicUpdateStep {
      using type = X;
      //**Info method*******************************************************
      static std::string info(Sim const&) noexcept {
        return "--Apply pre dynamic update\n";
      }
      //********************************************************************
    };
    struct PostDynamicUpdateStep {
      using type = X;
      //**Info method*******************************************************
      static std::string info(Sim const&) noexcept {
        return "--Apply post dynamic update\n";
      }
      //********************************************************************
    };

    using PostKinematicUpdateSteps =
        tmpl::list<FirstPostKinematicUpdateStep, SecondPostKinematicUpdateStep>;
    //! Algorithms to be carried out before the dynamic update step
    using PreDynamicUpdateSteps = tmpl::list<PreDynamicUpdateStep>;
    //! Algorithms to be carried out after the dynamic update step
    using PostDynamicUpdateSteps = tmpl::list<PostDynamicUpdateStep>;
  };

  template <class SimTraits, class Sim>
  struct TestModule : public TestAlgorithms<SimTraits, Sim> {
    void finalize(::elastica::Time) noexcept {
      // do some nontrivial operation here to test finalize_modules
      ++i;
      i *= 2;
      // does not raise any execptions at all
    }

    int i = {0};
  };

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.info_methods",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<TestModule>();

    SECTION("info update") {
      std::ostringstream ss;
      ::elastica::widthstream os(80UL, ss);
      simulator.info_update(os);
      // we first need to flush widthstream buffer to the underlying stream
      // which is automaticalyl done if you encounter a new line.
      os << "\n";
      INFO(ss.str());
      CHECK(ss.str().find("Update") != std::string::npos);
    }
    SECTION("post kinematic updates") {
      std::stringstream ss;
      ::elastica::widthstream os(80UL, ss);
      simulator.info_post_kinematic_updates(os);
      INFO(ss.str());
      CHECK(ss.str().find("first post kinematic") != std::string::npos);
      CHECK(ss.str().find("second post kinematic") != std::string::npos);
    }
    SECTION("pre dynamic updates") {
      std::stringstream ss;
      ::elastica::widthstream os(80UL, ss);
      simulator.info_pre_dynamic_updates(os);
      INFO(ss.str());
      CHECK(ss.str().find("pre dynamic") != std::string::npos);
    }
    SECTION("post dynamic updates") {
      std::stringstream ss;
      ::elastica::widthstream os(80UL, ss);
      simulator.info_post_dynamic_updates(os);
      INFO(ss.str());
      CHECK(ss.str().find("post dynamic") != std::string::npos);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.print_algorithm_to",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<TestModule>();
    std::ostringstream ss;
    simulator.stream_algorithm_to(ss);
    INFO(ss.str());
    std::vector<std::string> substrings{"Update", "first post kinematic",
                                        "second post kinematic", "pre dynamic",
                                        "post dynamic"};
    for (auto const& substr : substrings) {
      INFO(substr);
      CHECK(ss.str().find(substr) != std::string::npos);
    }
  }

  template <class SimTraits, class Sim>
  struct TestModuleWithException : public TestAlgorithms<SimTraits, Sim> {
    void finalize(::elastica::Time) {
      // do some nontrivial operation here to test finalize_modules
      throw std::runtime_error("Module throws exception!");
    }
  };

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.finalize_modules",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<TestModule>();
    CHECK(simulator.i == 0);
    CHECK_NOTHROW(simulator.finalize_modules(Time{0.0}));
    CHECK(simulator.i == 2);

    auto& another_simulator = make_test_simulator<TestModuleWithException>();
    CHECK_THROWS_NESTED_WITH(another_simulator.finalize_modules(Time{0.0}),
                             "Preparing the simulator",

                             "Module throws exception");
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.getters",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<TestModule>();
    using Sim = typename std::remove_reference_t<decltype(simulator)>;

    auto checker = [&](auto v) {
      using UpdateStep = tmpl::type_from<decltype(v)>;

      (void)(simulator.template get_timer<UpdateStep>());
      (void)(simulator.template get_algorithm<UpdateStep>());
    };
    SECTION("Check algorithms and timers") {
      tmpl::for_each<typename Sim::AlgorithmSteps>(checker);
    }
    SECTION("Check batching algorithms") {
      using Batcher = tmpl::type_<typename Sim::ConnectionFillerBatchAlgorithm>;
      (void)(simulator.template get_batching_algorithm<Batcher>());
    }
    // FirstPostKinematicUpdateStep;
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.profiling",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<TestModule>();

    Time time_v{0.0};
    for (int i = 0; i < 20; ++i) {
      simulator.apply_post_dynamic_updates(time_v);
      simulator.apply_post_kinematic_updates(time_v);
      simulator.apply_pre_dynamic_updates(time_v);
      // note that we technically dont need this as there are no systems, byt
      // without it the average time doesnt work, as the number of samples is
      // 0.
      simulator.for_each_system([](auto& /*b*/) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5ms);
      });
    }
    std::ostringstream ss;
    simulator.stream_profiling_summary_to(ss);
    INFO(ss.str());
    std::vector<std::string> substrings{
        "ApplyOverSystems", "FirstPostKinematicUpdateStep",
        "SecondPostKinematicUpdateStep", "PreDynamicUpdateStep",
        "PostDynamicUpdateStep"};
#ifdef ELASTICA_PROFILING
    for (auto const& substr : substrings) {
      INFO(substr);
      CHECK(ss.str().find(substr) != std::string::npos);
    }
#endif
  }

  template <class SimTraits, class Sim>
  class TestModuleResetNoExcept : public TestAlgorithms<SimTraits, Sim> {
   protected:
    void reset(::elastica::Time) noexcept {
      // do some nontrivial operation here to test reset
      ++i;
      i *= 2;
      // does not raise any execptions at all
    }

   public:
    int i = {0};
  };

  template <class SimTraits, class Sim>
  class TestModuleResetWithException : public TestAlgorithms<SimTraits, Sim> {
   protected:
    void reset(::elastica::Time) {
      throw std::runtime_error("Module throws exception!");
    }
  };

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Simulator.reset",
                     "[Unit][Simulator][Interface]") {
    // clang-format on
    auto& simulator = make_test_simulator<TestModuleResetNoExcept>();

    CHECK(simulator.i == 0);
    CHECK_NOTHROW(simulator.reset_modules(Time{0.0}));
    CHECK(simulator.i == 2);

    auto& another_simulator =
        make_test_simulator<TestModuleResetWithException>();
    CHECK_THROWS_NESTED_WITH(another_simulator.reset_modules(Time{0.0}),
                             "Resetting the simulator",
                             "Module throws exception");
  }
}  // namespace
