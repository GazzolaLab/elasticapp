//
// Created by Tejaswin Parthasarathy on 6/24/21.
//
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
///
#include "Simulator/Configuration.hpp"
#include "Simulator/Interface/Forcing/Module.hpp"
#include "Simulator/Interface/Indexers.hpp"
#include "Simulator/Interface/Protocols/Forcing.hpp"
/// define forcings
#include "Simulator/Forcing.hpp"  // for all types
///
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/Cpp20.hpp"
/// mocks
#include "Simulator/Interface/Forcing/MockForcing.hpp"
#include "Simulator/Interface/MockSimulator.hpp"
#include "Simulator/Interface/MockSystems.hpp"
/// serializatoin
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/MockSimulatorSerialize.hpp"

namespace {
  using namespace ::elastica::interface::forcing;
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultForcingConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ForcingModule>(
        sim_config, arg_all_registered, is_final);
    elastica::ignore_unused(sim);

    static_assert(
        ::tt::assert_conforms_to<decltype(sim),
                                 ::elastica::interface::protocols::Forcing>,
        "Meta failure");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module.check_if_system_can_be_forced",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultForcingConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ForcingModule>(
        sim_config, arg_all_registered, is_final);

    {
      CHECK_NOTHROW(sim.check_if_system_can_be_forced(float{}));
      CHECK_NOTHROW(sim.check_if_system_can_be_forced(char{}));
      CHECK_NOTHROW(sim.check_if_system_can_be_forced(double{}));
      // check references are propagated correctly
      CHECK_NOTHROW(
          sim.check_if_system_can_be_forced(std::unique_ptr<char>{nullptr}));
    }
    {
      sim.all_registered = false;
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_forced(float{}),
                               "Attempts to force system",
                               "Input system was not found");
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_forced(double{}),
                               "Attempts to force system",
                               "Input system was not found");
      CHECK_THROWS_NESTED_WITH(
          sim.check_if_system_can_be_forced(std::unique_ptr<char>{nullptr}),
          "Attempts to force system", "Input system was not found");
    }
    {
      sim.is_final = true;
      // no need to xor all_registered, because final() check is done first.
      // for a finaled() simulation, check the proper error
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_forced(float{}),
                               "Attempts to force system",
                               "simulator has been finalized");
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_forced(double{}),
                               "Attempts to force system",
                               "simulator has been finalized");
      CHECK_THROWS_NESTED_WITH(
          sim.check_if_system_can_be_forced(std::unique_ptr<char>{nullptr}),
          "Attempts to force system", "simulator has been finalized");
    }
  }

  template <typename ForcingSimulator>
  void check_force_interface(ForcingSimulator& sim) {
    // do not have std::size_t as a system because it is the index!
    using TestSystems =
        tmpl::list<tmpl::type_<float>, tmpl::type_<char>, tmpl::type_<double>>;
    tmpl::for_each<TestSystems>([&, n_cons = 0UL](auto v) mutable {
      using system_type = tmpl::type_from<decltype(v)>;

      system_type sys{};
      (void)sim.force(sys);
      CHECK(sim.n_forcings() == ++n_cons);
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module.force_interface",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultForcingConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;

    {
      auto sim = mock::make_simulator<::mock::ForcingModule>(
          sim_config, arg_all_registered, is_final);
      // check if a forceion object is returned
      using system_type = tmpl::type_<int>;
      system_type sys{};
      auto&& forcing = sim.force(sys);
      static_assert(cpp17::is_same_v<cpp20::remove_cvref_t<decltype(forcing)>,
                                     Forcing<system_type, tmpl::list<>>>,
                    "Forcing meta failure!");
    }

    {
      auto sim = mock::make_simulator<::mock::ForcingModule>(
          sim_config, arg_all_registered, is_final);
      check_force_interface(sim);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module.check_and_report_errors",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultForcingConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ForcingModule>(
        sim_config, arg_all_registered, is_final);

    check_force_interface(sim);

    // there are many half-baked forcings here, so we see error reporting
    // here
    CHECK_THROWS_WITH(sim.check_and_report_errors(),
                      Catch::Contains("Faults detected ") &&
                          Catch::Contains("listed by their"));
  }

}  // namespace

// to test whether the finalize() method calls all algorithms that are
// registered in the simulator, we can increment a common counter in all
// the defaulted connnections
struct Counter {
  Counter& operator++() noexcept {
    ++count;
    return *this;
  }
  std::size_t count = 0UL;
} force_counter_in_forcing,
    torque_counter_in_forcing;  // a global variable in this scope

// define some forcings
// not as intense as Test_Forcing in Simulator/Forcing or
// Simulator/Interface/Forcing
namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingDefaulted) {
     public:
      ForcingDefaulted() = default;
      ForcingDefaulted(ForcingDefaulted &&) noexcept {};
      ~ForcingDefaulted() = default;
      // move constructor is deleted if a destructor is defined
      //      ~ForcingDefaulted(){
      //        std::cout << "Destructor caleld!" << std::endl;
      //      }
      template <typename FirstSystem>
      inline void change_with_noop(FirstSystem & first_system) const {
        // see if first and second system can be changed
        first_system.n_indices += 1UL;
        first_system.n_indices -= 1UL;
      }

      template <typename TimeType, typename System>
      void apply_forces(TimeType /* time */, System & first_system,
                        std::size_t /* first_index */
      ) const {
        change_with_noop(first_system);
        ++force_counter_in_forcing;
      }
      template <typename TimeType, typename System>
      void apply_torques(TimeType /* time */, System & first_system,
                         std::size_t /* first_index */
      ) const {
        change_with_noop(first_system);
        ++torque_counter_in_forcing;
      }
      std::size_t some_member{23UL};
    };

    ELASTICA_DEFINE_FORCING(ForcingWithMember) {
     public:
      ForcingWithMember(int i, char c) : int_member(i), char_member(c){};
      ForcingWithMember(ForcingWithMember && other) noexcept
          : int_member(other.int_member), char_member(other.char_member){};
      ~ForcingWithMember() = default;

      template <typename TimeType, typename System>
      void apply_forces(TimeType /* time */, System& /*first_system*/,
                        std::size_t /* first_index */
      ) const {}
      template <typename TimeType, typename System>
      void apply_torques(TimeType /* time */, System& /*first_system*/,
                         std::size_t /* first_index */
      ) const {}
      explicit operator int() const noexcept { return int_member; }
      explicit operator char() const noexcept { return char_member; }

      int int_member;  // not 0
      float char_member;
    };

  }  // namespace forcing

}  // namespace elastica

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module.check_full_interface",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultForcingConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ForcingModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_forcings() == 0UL);

    using ::elastica::end;
    using ::elastica::forcing::ForcingDefaulted;
    using ::elastica::interface::at;
    using ::mock::MockSystem;
    using ExampleForcing = ForcingDefaulted<>;

    std::size_t expected_count = 0UL;

    {
      using System = MockSystem;

      /// [force_setup]
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      System sys{100UL};
      std::string optional_forcing_name = "ExampleForcing";
      // ExampleForcing is some elastica::forcing satisfying the
      // forcing protocols
      /// [force_setup]

      /// [force_eg]
      // order of with() and at() don't matter
      sim.force(sys, optional_forcing_name)
          .with<ExampleForcing>(/* ExampleForcing arguments*/)
          .at(1UL, 2UL, 3UL);
      sim.force(sys, std::move(optional_forcing_name))
          .at(5UL, end - 3UL)
          .with<ExampleForcing>(/* ExampleForcing arguments*/);
      /// [force_eg]

      CHECK(sim.n_forcings() == 2UL);
      expected_count += 5UL;
    }
    {
      using System = MockSystem;

      System sys{100UL};
      std::string optional_forcing_name = "ExampleForcing";

      /// [force_eg_no_curry]
      sim.force(sys, std::move(optional_forcing_name))
          .with(ExampleForcing{}, at(1UL, 2UL, 5UL, 17UL));
      /// [force_eg_no_curry]

      CHECK(sim.n_forcings() == 3UL);
      expected_count += 4UL;
    }
    {
      using System = MockSystem;

      System sys{100UL};
      std::string optional_forcing_name = "ExampleForcing";

      /// [force_eg_curry]
      // order of with() and at() don't matter
      sim.force(sys, optional_forcing_name)
          .with(ExampleForcing{})
          .at(end - 2UL, end - 1UL);
      sim.force(sys, std::move(optional_forcing_name))
          .at(end - 50UL, 23UL, 64UL)
          .with(ExampleForcing{});
      /// [force_eg_curry]

      CHECK(sim.n_forcings() == 5UL);
      expected_count += 5UL;
    }
    {
      using System = MockSystem;

      System sys{100UL};
      std::string optional_forcing_name = "ExampleForcing";
      /// [force_eg_with_name]
      // order of with() and at() don't matter
      // but put the with_name() immediately after launching force()
      sim.force(sys)
          .with_name((optional_forcing_name))
          .with(ExampleForcing{})
          .at(end - 50UL, 32UL, 64UL);
      sim.force(sys)
          .with_name(std::move(optional_forcing_name))
          .at(end - 50UL, 32UL, 64UL)
          .with(ExampleForcing{});
      /// [force_eg_with_name]

      CHECK(sim.n_forcings() == 7UL);
      expected_count += 6UL;
    }

    // now check the finalize() method and check the number of forcers
    {
      CHECK(force_counter_in_forcing.count == 0UL);
      CHECK(torque_counter_in_forcing.count == 0UL);

      // finalize
      CHECK_NOTHROW(sim.finalize());

      CHECK(force_counter_in_forcing.count == expected_count);
      CHECK(torque_counter_in_forcing.count == expected_count);
      CHECK(sim.forcers().size() == 7UL);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module.exception_for_finalize",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultForcingConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ForcingModule>(
        sim_config, arg_all_registered, is_final);

    CHECK(sim.n_forcings() == 0UL);

    using ::elastica::forcing::ForcingDefaulted;
    using ::mock::MockSystem;
    using ExampleForcing = ForcingDefaulted<>;

    {
      using System = MockSystem;
      System sys{100UL};
      // oops i forgot indices
      sim.force(sys).with<ExampleForcing>();
      // oops i forgot indices
      sim.force(sys).with(ExampleForcing{});
      // oops i forgot forcing
      sim.force(sys).at(6UL, 45UL);
      // WTF! This piece of code crashes the system due to RAM usage
      // I am HackerMan now by running elastica on clusters with this
      // code.
      //      // oops i forgot forceion
      //      sim.force(fs, end - 5UL).to(ss, end - 20UL);
    }
    CHECK_THROWS_NESTED_WITH(sim.finalize(),
                             "Preparing the forcing module failed",
                             "Faults detected ", "listed by their");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module.query",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultForcingConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ForcingModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_forcings() == 0UL);

    using ::elastica::end;
    using ::elastica::forcing::ForcingDefaulted;
    using ::elastica::interface::at;
    using ::elastica::interface::pair;
    using ::mock::MockSystem;
    using ExampleForcing = ForcingDefaulted<>;
    {
      using System = MockSystem;

      // same setup as earlier
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      System sys{100UL};
      std::string optional_forcing_name = "MyForcing";

      /// [query_eg]
      auto my_forcing_key =
          sim.force(sys, optional_forcing_name)
              .with<ExampleForcing>(/* ExampleForcing arguments*/)
              .at(end - 2UL, end - 1UL);

      sim.finalize();

      auto& my_forcing = sim.query(my_forcing_key);
      // you can now access member data of `my_forcing`
      /// [query_eg]

      CHECK(my_forcing.some_member == 23UL);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module.query_for_multiple",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultForcingConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ForcingModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_forcings() == 0UL);

    using ::elastica::end;
    using ::elastica::forcing::ForcingDefaulted;
    using ::elastica::forcing::ForcingWithMember;
    using ::elastica::interface::at;
    using ::elastica::interface::pair;
    using ::mock::MockSystem;
    {
      using System = MockSystem;
      using ExampleForcing = ForcingDefaulted<>;
      using MemberForcing = ForcingWithMember<>;

      // same setup as earlier
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      System sys{100UL};
      std::string optional_forcing_name = "MyForcing";

      auto first_forcing_key =
          sim.force(sys, optional_forcing_name)
              .with<ExampleForcing>(/* ExampleForcing arguments*/)
              .at(end - 2UL, end - 1UL);
      auto second_forcing_key = sim.force(sys, optional_forcing_name)
                                    .with<MemberForcing>(32, 'a')
                                    .at(1UL);
      auto third_forcing_key =
          sim.force(sys, optional_forcing_name)
              .with<ExampleForcing>(/* ExampleForcing arguments*/)
              .at(2UL);
      auto fourth_forcing_key = sim.force(sys, optional_forcing_name)
                                    .with<MemberForcing>(42, 'd')
                                    .at(end - 1UL);

      sim.finalize();

      auto& first_forcing = sim.query(first_forcing_key);
      auto& third_forcing = sim.query(third_forcing_key);

      CHECK(first_forcing.some_member == 23UL);
      CHECK(third_forcing.some_member == 23UL);

      auto& second_forcing = sim.query(second_forcing_key);
      auto& fourth_forcing = sim.query(fourth_forcing_key);
      CHECK(int(second_forcing) == 32);
      CHECK(char(second_forcing) == 'a');
      CHECK(int(fourth_forcing) == 42);
      CHECK(char(fourth_forcing) == 'd');
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Module.serialize",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    using namespace elastica::io;
    using ::elastica::forcing::ForcingDefaulted;
    using ::mock::MockSystem;
    using ExampleForcing = ForcingDefaulted<>;
    using ::elastica::end;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        auto sim_config =
            mock::make_simulator_configuration(DefaultForcingConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ForcingModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of forcings
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.force(sys)                     //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleForcing{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config =
            mock::make_simulator_configuration(DefaultForcingConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ForcingModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of forcings
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.force(sys)                     //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleForcing{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(iarchive(sim));
      }
    }

    // throwing case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        auto sim_config =
            mock::make_simulator_configuration(DefaultForcingConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ForcingModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of forcings
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.force(sys)                     //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleForcing{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config =
            mock::make_simulator_configuration(DefaultForcingConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ForcingModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of forcings
          // added 5 less forcings!
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.force(sys)                     //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleForcing{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_THROWS_NESTED_WITH(iarchive(sim), "Loading the Simulator state",
                                 "Number of forcings from");
      }
    }

    // throwing case, but this time the number of forcings are the same,
    // but some forcings are different
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        auto sim_config =
            mock::make_simulator_configuration(DefaultForcingConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ForcingModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of forcings
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.force(sys)                     //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleForcing{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config =
            mock::make_simulator_configuration(DefaultForcingConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ForcingModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of forcings
          // added 5 less forcings!
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.force(sys)                     //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleForcing{});

          struct AnotherForcingTag {};
          using AnotherForcing =
              tmpl::inherit<ExampleForcing, AnotherForcingTag>::type;
          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.force(sys)                     //
                .at(10UL + i, end - 10UL - i)  //
                .with(AnotherForcing{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_THROWS_NESTED_WITH(iarchive(sim), "Loading the Simulator state",
                                 "Restored UUID", "does not match");
      }
    }
  }

}  // namespace
