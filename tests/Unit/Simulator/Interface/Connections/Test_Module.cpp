//
// Created by Tejaswin Parthasarathy on 6/20/21.
//

#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
///
#include "Simulator/Configuration.hpp"
#include "Simulator/Interface/Connections/Module.hpp"
#include "Simulator/Interface/Indexers.hpp"
#include "Simulator/Interface/Protocols/Connections.hpp"
/// define connections
#include "Simulator/Connections.hpp"  // for all types
///
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/Cpp20.hpp"
/// mocks
#include "Simulator/Interface/Connections/MockConnects.hpp"
#include "Simulator/Interface/MockSimulator.hpp"
#include "Simulator/Interface/MockSystems.hpp"
/// serializatoin
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/MockSimulatorSerialize.hpp"

// check start to end for one connection
// simulator->connect(a, b).with().at();
// simulator->connect(a).to(b).with().at();
// simulator->connect(a).at().to().at().with().
// simulator->connect(a, at(2UL)).to(b, at(4UL)).with() ...

namespace {
  using namespace ::elastica::interface::connections;
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);
    elastica::ignore_unused(sim);

    static_assert(
        ::tt::assert_conforms_to<decltype(sim),
                                 ::elastica::interface::protocols::Connections>,
        "Meta failure");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module.check_if_systems_can_be_connected",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);

    {
      CHECK_NOTHROW(sim.check_if_systems_can_be_connected(int{}, float{}));
      CHECK_NOTHROW(sim.check_if_systems_can_be_connected(float{}, char{}));
      CHECK_NOTHROW(sim.check_if_systems_can_be_connected(double{}, double{}));
      // check references are propagated correctly
      CHECK_NOTHROW(sim.check_if_systems_can_be_connected(
          std::unique_ptr<double>{nullptr}, std::unique_ptr<char>{nullptr}));
    }
    {
      sim.all_registered = false;
      CHECK_THROWS_NESTED_WITH(
          sim.check_if_systems_can_be_connected(int{}, float{}),
          "Attempts to connect systems", "Input system was not found");
      CHECK_THROWS_NESTED_WITH(
          sim.check_if_systems_can_be_connected(char{}, double{}),
          "Attempts to connect systems", "Input system was not found");
      CHECK_THROWS_NESTED_WITH(
          sim.check_if_systems_can_be_connected(
              std::unique_ptr<double>{nullptr}, std::unique_ptr<char>{nullptr}),
          "Attempts to connect systems", "Input system was not found");
    }
    {
      sim.is_final = true;
      // no need to xor all_registered, because final() check is done first.
      // for a finaled() simulation, check the proper error
      CHECK_THROWS_NESTED_WITH(
          sim.check_if_systems_can_be_connected(int{}, float{}),
          "Attempts to connect systems", "simulator has been finalized");
      CHECK_THROWS_NESTED_WITH(
          sim.check_if_systems_can_be_connected(char{}, double{}),
          "Attempts to connect systems", "simulator has been finalized");
      CHECK_THROWS_NESTED_WITH(
          sim.check_if_systems_can_be_connected(
              std::unique_ptr<double>{nullptr}, std::unique_ptr<char>{nullptr}),
          "Attempts to connect systems", "simulator has been finalized");
    }
  }

  template <typename ConnectionSimulator>
  void check_connect_interface(ConnectionSimulator& sim) {
    // do not have std::size_t as a system because it is the index!
    using TestSystems =
        tmpl::list<tmpl::pair<tmpl::type_<int>, tmpl::type_<float>>,
                   tmpl::pair<tmpl::type_<float>, tmpl::type_<char>>,
                   tmpl::pair<tmpl::type_<std::size_t>, tmpl::type_<double>>>;
    tmpl::for_each<TestSystems>([&, n_conn = 0UL](auto v) mutable {
      using pair = tmpl::type_from<decltype(v)>;
      using first_type = typename pair::first_type;
      using second_type = typename pair::second_type;

      first_type f{};
      second_type s{};
      (void)sim.connect(f, s);
      CHECK(sim.n_connections() == ++n_conn);
    });
  }

  // clang-format off
ELASTICA_TEST_CASE(
    "Unit.Simulator.Interface.Connections.Module.connect_interface",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;

    {
      auto sim = mock::make_simulator<::mock::ConnectionsModule>(
          sim_config, arg_all_registered, is_final);
      // check if a connection object is returned
      using first_type = int;
      using second_type = float;
      first_type f{};
      second_type s{};
      auto&& connection = sim.connect(f, s);
      static_assert(
          cpp17::is_same_v<cpp20::remove_cvref_t<decltype(connection)>,
                           Connection<int, float, tmpl::list<>>>,
          "Connection meta failure!");
    }

    {
      auto sim = mock::make_simulator<::mock::ConnectionsModule>(
          sim_config, arg_all_registered, is_final);
      check_connect_interface(sim);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module.check_and_report_errors",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);

    check_connect_interface(sim);

    // there are many half-baked connections here, so we see error reporting
    // here
    CHECK_THROWS_WITH(sim.check_and_report_errors(),
                      Catch::Contains("Faults detected ") &&
                          Catch::Contains("listed by their"));
  }

}  // namespace

// to test whether the finalize() method calls all algorithms that are
// registered in the simulator, we can increment a common counter in all
// the defaulted connnections
struct ConnectionsCounter {
  ConnectionsCounter& operator++() noexcept {
    ++count;
    return *this;
  }
  std::size_t count = 0UL;
} force_counter, torque_counter;  // a global variable in this scope

// define some connections
// not as intense as Test_Connection in Simulator/Connections or
// Simulator/Interface/Connections
namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionDefaulted) {
     public:
      ConnectionDefaulted(std::size_t some_arg = 23UL)
          : some_member{some_arg} {}
      // Needs to be properly defined for query to work
      ConnectionDefaulted(ConnectionDefaulted &&) {}
      // If I do this, the counter doesn't work :/
      /*
      ConnectionDefaulted(ConnectionDefaulted && other)
          : some_member{std::move(other.some_member)} {};
          */

      ~ConnectionDefaulted() = default;
      template <typename FirstSystem, typename SecondSystem>
      inline void change_with_noop(FirstSystem & first_system,
                                   SecondSystem & second_system) const {
        // see if first and second system can be changed
        first_system.n_indices += 1UL;
        first_system.n_indices -= 1UL;
        second_system.n_indices += 1UL;
        second_system.n_indices -= 1UL;
      }

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem & first_system,
                        std::size_t /* first_index */,
                        SecondSystem & second_system,
                        std::size_t /* second_index */) const {
        change_with_noop(first_system, second_system);
        ++force_counter;
      }
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem & first_system,
                        SecondSystem & second_system) const {
        change_with_noop(first_system, second_system);
        ++force_counter;
      }

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem & first_system,
                         std::size_t /* first_index */,
                         SecondSystem & second_system,
                         std::size_t /* second_index */) const {
        change_with_noop(first_system, second_system);
        ++torque_counter;
      }
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem & first_system,
                         SecondSystem & second_system) const {
        change_with_noop(first_system, second_system);
        ++torque_counter;
      }

     public:
      std::size_t some_member;  // {23UL};  // not zero!
    };

    ELASTICA_DEFINE_CONNECTION(ConnectionWithMember) {
     public:
      ConnectionWithMember(int i, char c) : int_member(i), char_member(c){};
      ConnectionWithMember(ConnectionWithMember && other) noexcept
          : int_member(other.int_member), char_member(other.char_member){};
      ~ConnectionWithMember() = default;

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /*first_system*/,
                        std::size_t /* first_index */,
                        SecondSystem& /*second_system*/,
                        std::size_t /* second_index */) const {}
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(
          TimeType /* time */, FirstSystem& /*first_system*/, SecondSystem &
          /*second_system*/) const {}

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /*first_system*/,
                         std::size_t /* first_index */,
                         SecondSystem& /*second_system*/,
                         std::size_t /* second_index */) const {}
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(
          TimeType /* time */, FirstSystem& /*first_system*/, SecondSystem &
          /*second_system*/) const {}
      explicit operator int() const noexcept { return int_member; }
      explicit operator char() const noexcept { return char_member; }

     public:
      int int_member;  // not 0
      char char_member;
    };

    ELASTICA_DEFINE_CONNECTION(AnotherConnectionWithMember) {
     public:
      AnotherConnectionWithMember(char c, float f, double d)
          : char_member(c), float_member(f), double_member(d){};
      AnotherConnectionWithMember(AnotherConnectionWithMember && other) noexcept
          : char_member(other.char_member),
            float_member(other.float_member),
            double_member(other.double_member) {
        std::cout << "Move constructor called" << std::endl;
      };
      ~AnotherConnectionWithMember() = default;

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /*first_system*/,
                        std::size_t /* first_index */,
                        SecondSystem& /*second_system*/,
                        std::size_t /* second_index */) const {}
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(
          TimeType /* time */, FirstSystem& /*first_system*/, SecondSystem &
          /*second_system*/) const {}

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /*first_system*/,
                         std::size_t /* first_index */,
                         SecondSystem& /*second_system*/,
                         std::size_t /* second_index */) const {}
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(
          TimeType /* time */, FirstSystem& /*first_system*/, SecondSystem &
          /*second_system*/) const {}
      //      explicit operator int() const noexcept { return int_member; }
      //      explicit operator char() const noexcept { return char_member; }

     public:
      char char_member;
      float float_member;
      double double_member;
    };

  }  // namespace connections

}  // namespace elastica

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module.get_symbolic_index",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);
    using Sim = decltype(sim);
    elastica::ignore_unused(sim);

    // no transform simple bois
    static_assert(Sim::get_symbolic_index(2UL) == 2UL,
                  "symbolic index meta failure!");
    static_assert(Sim::get_symbolic_index(::elastica::end - 40UL) ==
                      ::elastica::from_end{40UL},
                  "symbolic index meta failure!");
    using ::elastica::interface::at;
    // using ::elastica::interface::detail::At_t;

    CHECK(Sim::get_symbolic_index(at(2UL)) == 2UL);
    CHECK(Sim::get_symbolic_index(at(::elastica::end - 49UL)) ==
          ::elastica::from_end{49UL});
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module.check_full_interface",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_connections() == 0UL);

    using ::elastica::end;
    using ::elastica::connections::ConnectionDefaulted;
    using ::elastica::interface::at;
    using ::elastica::interface::pair;
    using ::mock::MockSystem;
    using ExampleConnection = ConnectionDefaulted<>;
    {
      using first_system = MockSystem;
      using second_system = MockSystem;

      /// [connect_setup]
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      first_system fs{100UL};
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      second_system ss{100UL};
      std::string optional_connection_name = "ExampleConnection";
      // ExampleConnection is some elastica::connection satisfying the
      // connection protocols
      /// [connect_setup]

      /// [connect_eg]
      // order of with() and at() don't matter
      sim.connect(fs, ss, optional_connection_name)
          .with<ExampleConnection>(/* ExampleConnection arguments*/)
          .at(pair(1UL, 2UL));
      sim.connect(fs, ss, std::move(optional_connection_name))
          .at(pair(end - 3UL, 5UL))
          .with<ExampleConnection>(/* ExampleConnection arguments*/);
      /// [connect_eg]

      CHECK(sim.n_connections() == 2UL);
      sim.connect(fs, ss).with<ConnectionDefaulted<>>().at(
          pair(2UL, end - 40UL));
      CHECK(sim.n_connections() == 3UL);
      sim.connect(fs, ss).with<ConnectionDefaulted<>>().at(
          pair(end - 2UL, end - 49UL));
      CHECK(sim.n_connections() == 4UL);
    }
    {
      using first_system = MockSystem;
      using second_system = MockSystem;

      first_system fs{100UL};
      second_system ss{100UL};
      std::string optional_connection_name = "ExampleConnection";

      /// [connect_eg_no_curry]
      sim.connect(fs, ss, std::move(optional_connection_name))
          .with(ExampleConnection{}, at(pair(1UL, 2UL)));
      /// [connect_eg_no_curry]

      CHECK(sim.n_connections() == 5UL);

      sim.connect(fs, ss).with(ConnectionDefaulted<>{},
                               at(pair(end - 1UL, 5UL)));
      CHECK(sim.n_connections() == 6UL);

      sim.connect(fs, ss).with(ConnectionDefaulted<>{},
                               at(pair(5UL, end - 4UL)));
      CHECK(sim.n_connections() == 7UL);

      sim.connect(fs, ss).with(ConnectionDefaulted<>{},
                               at(pair(end - 25UL, end - 4UL)));
      CHECK(sim.n_connections() == 8UL);
    }
    {
      using first_system = MockSystem;
      using second_system = MockSystem;

      first_system fs{100UL};
      second_system ss{100UL};
      std::string optional_connection_name = "ExampleConnection";

      /// [connect_eg_curry]
      // order of with() and at() don't matter
      sim.connect(fs, ss, optional_connection_name)
          .with(ExampleConnection{})
          .at(pair(1UL, 2UL));
      sim.connect(fs, ss, std::move(optional_connection_name))
          .at(pair(end - 50UL, 23UL))
          .with(ExampleConnection{});
      /// [connect_eg_curry]

      CHECK(sim.n_connections() == 10UL);

      sim.connect(fs, ss)
          .with(ConnectionDefaulted<>{})
          .at(pair(6UL, end - 9UL));
      CHECK(sim.n_connections() == 11UL);

      sim.connect(fs, ss)
          .with(ConnectionDefaulted<>{})
          .at(pair(end - 42UL, end - 9UL));
      CHECK(sim.n_connections() == 12UL);
    }
    {
      using first_system = MockSystem;
      using second_system = MockSystem;
      first_system fs{100UL};
      second_system ss{100UL};
      std::string optional_connection_name = "ExampleConnection";

      /// [connect_to_eg]
      // order of with(), at(), with_name() don't matter
      // but to() should follow connect() for readability
      sim.connect(fs)
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .with<ExampleConnection>(/* ExampleConnection arguments*/)
          .at(pair(1UL, 2UL));
      sim.connect(fs)
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .at(pair(end - 7UL, 9UL))
          .with<ExampleConnection>(/* ExampleConnection arguments*/);
      /// [connect_to_eg]

      CHECK(sim.n_connections() == 14UL);

      sim.connect(fs)
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .at(pair(9UL, end - 7UL))
          .with<ExampleConnection>(/* ExampleConnection arguments*/);
      CHECK(sim.n_connections() == 15UL);

      sim.connect(fs)
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .at(pair(end - 82UL, end - 7UL))
          .with<ExampleConnection>(/* ExampleConnection arguments*/);
      CHECK(sim.n_connections() == 16UL);

      /// [connect_to_eg_no_curry]
      sim.connect(fs)  //
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .with(ExampleConnection{}, at(pair(1UL, 2UL)));
      /// [connect_to_eg_no_curry]

      CHECK(sim.n_connections() == 17UL);

      sim.connect(fs)  //
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .with(ExampleConnection{}, at(pair(end - 1UL, 2UL)));
      CHECK(sim.n_connections() == 18UL);

      sim.connect(fs)  //
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .with(ExampleConnection{}, at(pair(5UL, end - 11UL)));
      CHECK(sim.n_connections() == 19UL);

      sim.connect(fs)  //
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .with(ExampleConnection{}, at(pair(end - 51UL, end - 11UL)));
      CHECK(sim.n_connections() == 20UL);

      /// [connect_to_eg_curry]
      // order of with() and at() don't matter
      // but to() should follow connect() for readability
      sim.connect(fs)  //
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .with(ExampleConnection{})
          .at(pair(1UL, 2UL));
      sim.connect(fs)  //
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .at(pair(1UL, end - 45UL))
          .with(ExampleConnection{});
      /// [connect_to_eg_curry]
      CHECK(sim.n_connections() == 22UL);

      sim.connect(fs)  //
          .to(ss)
          .with_name(optional_connection_name)  // can be skipped
          .at(pair(end - 6UL, 45UL))
          .with(ExampleConnection{});
      CHECK(sim.n_connections() == 23UL);

      sim.connect(fs)  //
          .to(ss)
          .with(ExampleConnection{})
          .at(pair(end - 6UL, end - 45UL));
      CHECK(sim.n_connections() == 24UL);
    }
    {
      using first_system = MockSystem;
      using second_system = MockSystem;
      first_system fs{100UL};
      second_system ss{100UL};
      std::string optional_connection_name = "ExampleConnection";

      /// [connect_idx_eg]
      sim.connect(fs, 5UL)  //
          .to(ss, 6UL)      //
          .with<ExampleConnection>(/* ExampleConnection arguments*/);
      // .with_name(optional_connection_name);  // cannot be named for now
      /// [connect_idx_eg]

      CHECK(sim.n_connections() == 25UL);

      /// [connect_idx_eg_curry]
      sim.connect(fs, 19UL)    //
          .to(ss, end - 20UL)  //
          .with(ExampleConnection{});
      // .with_name(optional_connection_name);  // cannot be named for now
      /// [connect_idx_eg_curry]

      CHECK(sim.n_connections() == 26UL);
    }
    {
      using first_system = MockSystem;
      using second_system = MockSystem;
      first_system fs{100UL};
      second_system ss{100UL};
      std::string optional_connection_name = "ExampleConnection";

      /// [connect_end_idx_eg]
      sim.connect(fs, end - 5UL)  //
          .to(ss, end - 6UL)      //
          .with<ExampleConnection>(/* ExampleConnection arguments*/);
      // .with_name(optional_connection_name);  // cannot be named for now
      /// [connect_end_idx_eg]
      CHECK(sim.n_connections() == 27UL);

      /// [connect_end_idx_eg_curry]
      sim.connect(fs, end - 5UL)  //
          .to(ss, 6UL)            //
          .with(ExampleConnection{});
      // .with_name(optional_connection_name);  // cannot be named for now
      /// [connect_end_idx_eg_curry]
      CHECK(sim.n_connections() == 28UL);
    }
    {
      using first_system = MockSystem;
      using second_system = MockSystem;
      first_system fs{100UL};
      second_system ss{100UL};
      std::string optional_connection_name = "ExampleConnection";

      /// [connect_at_idx_eg]
      sim.connect(fs, at(15UL))  //
          .to(ss, at(61UL))      //
          .with<ExampleConnection>(/* ExampleConnection arguments*/);
      // .with_name(optional_connection_name);  // cannot be named for now
      /// [connect_at_idx_eg]

      CHECK(sim.n_connections() == 29UL);

      /// [connect_at_idx_eg_curry]
      sim.connect(fs, at(49UL))    //
          .to(ss, at(end - 25UL))  //
          .with(ExampleConnection{});
      // .with_name(optional_connection_name);  // cannot be named for now
      /// [connect_at_idx_eg_curry]

      CHECK(sim.n_connections() == 30UL);
    }
    {
      using first_system = MockSystem;
      using second_system = MockSystem;
      first_system fs{100UL};
      second_system ss{100UL};
      std::string optional_connection_name = "ExampleConnection";

      /// [connect_at_end_idx_eg]
      sim.connect(fs, at(end - 15UL))  //
          .to(ss, at(end - 61UL))      //
          .with<ExampleConnection>(/* ExampleConnection arguments*/);
      // .with_name(optional_connection_name);  // cannot be named for now
      /// [connect_at_end_idx_eg]

      CHECK(sim.n_connections() == 31UL);

      /// [connect_at_end_idx_eg_curry]
      sim.connect(fs, at(end - 49UL))  //
          .to(ss, at(25UL))            //
          .with(ExampleConnection{});
      // .with_name(optional_connection_name);  // can be skipped
      /// [connect_at_end_idx_eg_curry]

      CHECK(sim.n_connections() == 32UL);
    }

    // now check the finalize() method and check the number of connecters
    {
      CHECK(force_counter.count == 0UL);
      CHECK(torque_counter.count == 0UL);

      // finalize
      CHECK_NOTHROW(sim.finalize());

      CHECK(force_counter.count == 32UL);
      CHECK(torque_counter.count == 32UL);
      CHECK(sim.connecters().size() == 32UL);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module.exception_for_finalize",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_connections() == 0UL);
    using ::elastica::connections::ConnectionDefaulted;
    using ::mock::MockSystem;
    using ExampleConnection = ConnectionDefaulted<>;

    {
      using first_system = MockSystem;
      using second_system = MockSystem;
      first_system fs{100UL};
      second_system ss{100UL};
      // oops i forgot indices
      sim.connect(fs, ss).with<ExampleConnection>();
      // oops i forgot indices
      sim.connect(fs).to(ss).with(ExampleConnection{});
      // oops i forgot connection
      sim.connect(fs, 5UL).to(ss, 6UL);
      // WTF! This piece of code crashes the system due to RAM usage
      // I am HackerMan now by running elastica on clusters with this
      // code.
      //      // oops i forgot connection
      //      sim.connect(fs, end - 5UL).to(ss, end - 20UL);
    }
    CHECK_THROWS_NESTED_WITH(sim.finalize(),
                             "Preparing the connections module failed",
                             "Faults detected ", "listed by their");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module.query",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_connections() == 0UL);

    using ::elastica::end;
    using ::elastica::connections::ConnectionDefaulted;
    using ::elastica::connections::ConnectionWithMember;
    using ::elastica::interface::at;
    using ::elastica::interface::pair;
    using ::mock::MockSystem;
    using Reg =
        tmpl::list<::elastica::connections::Registrars::ConnectionDefaulted,
                   ::elastica::connections::Registrars::ConnectionWithMember>;
    using ExampleConnection = ConnectionWithMember<Reg>;
    {
      using first_system = MockSystem;
      using second_system = MockSystem;

      // same setup as earlier
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      first_system fs{100UL};
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      second_system ss{100UL};
      std::string optional_connection_name = "MyConnection";
      // ExampleConnection is some elastica::connection satisfying the
      // connection protocols

      /// [query_eg]
      int first_argument = 21;
      char second_argument = 'y';
      auto my_connection_key =
          sim.connect(fs)
              .to(ss)
              .with_name(optional_connection_name)  // can be skipped
              .with<ExampleConnection>(first_argument, second_argument)
              .at(pair(1UL, 2UL));

      sim.finalize();

      auto& my_connection = sim.query(my_connection_key);
      // you can now access member data of `my_connection`
      /// [query_eg]

      // CHECK(my_connection.some_member == 23UL);
      CHECK(int(my_connection) == 21);
      CHECK(char(my_connection) == 'y');
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module.query_for_multiple",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_connections() == 0UL);

    using ::elastica::end;
    using ::elastica::connections::AnotherConnectionWithMember;
    using ::elastica::connections::ConnectionDefaulted;
    using ::elastica::connections::ConnectionWithMember;
    using ::elastica::interface::at;
    using ::elastica::interface::pair;
    using ::mock::MockSystem;
    // using ExampleConnection = ConnectionDefaulted<>;
    {
      using first_system = MockSystem;
      using second_system = MockSystem;

      // same setup as earlier
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      first_system fs{100UL};
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      second_system ss{100UL};
      std::string optional_connection_name = "MyConnection";
      // ExampleConnection is some elastica::connection satisfying the
      // connection protocols

      //      auto first_connection_key =
      //          sim.connect(fs)
      //              .to(ss)
      //              .with_name(optional_connection_name)  // can be skipped
      //              .with<ConnectionDefaulted<>>(43UL)
      //              .at(pair(1UL, 2UL));
      auto second_connection_key =
          sim.connect(fs)
              .to(ss)
              .with_name(optional_connection_name)  // can be skipped
              .with<ConnectionWithMember<>>(42, 'p')
              .at(pair(10UL, 21UL));
      //      auto third_connection_key =
      //          sim.connect(fs)
      //              .to(ss)
      //              .with_name(optional_connection_name)  // can be skipped
      //              .with<ConnectionDefaulted<>>(
      //                  /* ExampleConnection arguments*/)
      //              .at(pair(5UL, 12UL));
      auto fourth_connection_key =
          sim.connect(fs)
              .to(ss)
              .with_name(optional_connection_name)  // can be skipped
              .with<ConnectionWithMember<>>(35, 'q')
              .at(pair(10UL, 21UL));
      auto fifth_connection_key =
          sim.connect(fs)
              .to(ss)
              .with_name(optional_connection_name)  // can be skipped
              .with<AnotherConnectionWithMember<>>('a', 5.12F, 3.14159)
              .at(pair(12UL, 22UL));
      auto sixth_connection_key =
          sim.connect(fs)
              .to(ss)
              .with_name(optional_connection_name)  // can be skipped
              .with<AnotherConnectionWithMember<>>('c', 3.14F, 6.28318)
              .at(pair(18UL, 72UL));

      sim.finalize();

      //      auto& first_connection = sim.query(first_connection_key);
      //      auto& third_connection = sim.query(third_connection_key);
      //
      //      CHECK(first_connection.some_member == 43UL);
      //      CHECK(third_connection.some_member == 23UL);

      auto& second_connection = sim.query(second_connection_key);
      auto& fourth_connection = sim.query(fourth_connection_key);
      CHECK(int(second_connection) == 42);
      CHECK(char(second_connection) == 'p');
      CHECK(int(fourth_connection) == 35);
      CHECK(char(fourth_connection) == 'q');

      auto& fifth_connection = sim.query(fifth_connection_key);
      auto& sixth_connection = sim.query(sixth_connection_key);

      CHECK(fifth_connection.char_member == 'a');
      CHECK(fifth_connection.float_member == approx(5.12F));
      CHECK(fifth_connection.double_member == approx(3.14159));

      CHECK(sixth_connection.char_member == 'c');
      CHECK(sixth_connection.float_member == approx(3.14F));
      CHECK(sixth_connection.double_member == approx(6.28318));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Module.serialize",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using namespace elastica::io;
    using ::elastica::connections::ConnectionDefaulted;
    using ::mock::MockSystem;
    using ExampleConnection = ConnectionDefaulted<>;
    using ::elastica::end;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConnectionsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConnectionsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of connections
          using first_system = MockSystem;
          using second_system = MockSystem;
          first_system first{100UL};
          second_system second{100UL};
          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.connect(first, end - 10UL - i)  //
                .to(second, 10UL + i)           //
                .with(ExampleConnection{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConnectionsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConnectionsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of connections
          using first_system = MockSystem;
          using second_system = MockSystem;
          first_system first{100UL};
          second_system second{100UL};
          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.connect(first, end - 10UL - i)  //
                .to(second, 10UL + i)           //
                .with(ExampleConnection{});
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

        auto sim_config = mock::make_simulator_configuration(
            DefaultConnectionsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConnectionsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of connections
          using first_system = MockSystem;
          using second_system = MockSystem;
          first_system first{100UL};
          second_system second{100UL};
          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.connect(first, end - 10UL - i)  //
                .to(second, 10UL + i)           //
                .with(ExampleConnection{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConnectionsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConnectionsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of connections
          // added 5 less connections!
          using first_system = MockSystem;
          using second_system = MockSystem;
          first_system first{100UL};
          second_system second{100UL};
          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.connect(first, end - 10UL - i)  //
                .to(second, 10UL + i)           //
                .with(ExampleConnection{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_THROWS_NESTED_WITH(iarchive(sim), "Loading the Simulator state",
                                 "Number of connections from");
      }
    }

    // throwing case, but this time the number of connections are the same,
    // but some connections are different
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConnectionsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConnectionsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of connections
          using first_system = MockSystem;
          using second_system = MockSystem;
          first_system first{100UL};
          second_system second{100UL};
          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.connect(first, end - 10UL - i)  //
                .to(second, 10UL + i)           //
                .with(ExampleConnection{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConnectionsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConnectionsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of connections
          // added 5 less connections!
          using first_system = MockSystem;
          using second_system = MockSystem;
          first_system first{100UL};
          second_system second{100UL};
          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.connect(first, end - 10UL - i)  //
                .to(second, 10UL + i)           //
                .with(ExampleConnection{});

          struct AnotherConnectionTag {};
          using AnotherConnection =
              tmpl::inherit<ExampleConnection, AnotherConnectionTag>::type;
          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.connect(first, end - 10UL - i)  //
                .to(second, 10UL + i)           //
                .with(AnotherConnection{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_THROWS_NESTED_WITH(iarchive(sim), "Loading the Simulator state",
                                 "Restored UUID", "does not match");
      }
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.Module.reset",
                     "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConnectionsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConnectionsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_connections() == 0UL);

    using ::elastica::connections::ConnectionWithMember;
    using ::elastica::interface::at;
    using ::mock::MockSystem;

    using MemberConnection = ConnectionWithMember<>;
    {
      using first_system = MockSystem;
      using second_system = MockSystem;
      first_system first{100UL};
      second_system second{100UL};
      std::string optional_connection_name = "MyConnections";

      for (std::size_t i = 0UL; i < 5UL; ++i)
        sim.connect(first, at(5UL))
            .to(second, at(6UL))
            .with<MemberConnection>(32, 'a');
    }

    sim.finalize();

    CHECK(sim.n_connections() == 5UL);
    // CHECK(sim.ready()); we are not testing the mock simulator
    CHECK_FALSE(sim.connecters().empty());

    sim.reset_module(::elastica::Time{0.0});

    CHECK(sim.n_connections() == 0UL);
    // CHECK_FALSE(sim.ready());  we are not testing the mock simulator
    CHECK(sim.connecters().empty());
  }

}  // namespace

// [&, this, second_index =
// std::forward<decltype(second_index)>(second_index)]
//                  make_overloader(
//                  [&, this](std::size_t next_index) {
//                    return this->connect(first_system,
//                    second_system)
//                        .at(::elastica::interface::pair(index,
//                        next_index));
//                  },
//                  [&, this](::elastica::from_end next_index) {
//                    return this->connect(first_system,
//                    second_system)
//                        .at(::elastica::interface::pair(index,
//                        next_index));
//                  },
//                  [&, this](
//                      ::elastica::interface::detail::At_t<1,
//                      0>&& next_index) {
//                    return this->connect(first_system,
//                    second_system)
//                        .at(::elastica::interface::pair(
//                            index,
//                            std::get<0UL>(std::move(next_index.ints()))));
//                  })(std::move(second_index));
