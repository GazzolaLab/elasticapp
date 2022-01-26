//
// Created by Tejaswin Parthasarathy on 6/24/21.
//
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
///
#include "Simulator/Configuration.hpp"
#include "Simulator/Interface/Constraints/Module.hpp"
#include "Simulator/Interface/Indexers.hpp"
#include "Simulator/Interface/Protocols/Constraints.hpp"
/// define constraints
#include "Simulator/Constraints.hpp"  // for all types
///
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/Cpp20.hpp"
/// mocks
#include "Simulator/Interface/Constraints/MockConstraints.hpp"
#include "Simulator/Interface/MockSimulator.hpp"
#include "Simulator/Interface/MockSystems.hpp"
/// serializatoin
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/MockSimulatorSerialize.hpp"

namespace {
  using namespace ::elastica::interface::constraints;
  using namespace ::elastica::configuration;
}  // namespace

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConstraintsModule>(
        sim_config, arg_all_registered, is_final);
    elastica::ignore_unused(sim);

    static_assert(
        ::tt::assert_conforms_to<decltype(sim),
                                 ::elastica::interface::protocols::Constraints>,
        "Meta failure");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module.check_if_system_can_be_constrained",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConstraintsModule>(
        sim_config, arg_all_registered, is_final);

    {
      CHECK_NOTHROW(sim.check_if_system_can_be_constrained(float{}));
      CHECK_NOTHROW(sim.check_if_system_can_be_constrained(char{}));
      CHECK_NOTHROW(sim.check_if_system_can_be_constrained(double{}));
      // check references are propagated correctly
      CHECK_NOTHROW(sim.check_if_system_can_be_constrained(
          std::unique_ptr<char>{nullptr}));
    }
    {
      sim.all_registered = false;
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_constrained(float{}),
                               "Attempts to constrain system",
                               "Input system was not found");
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_constrained(double{}),
                               "Attempts to constrain system",
                               "Input system was not found");
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_constrained(
                                   std::unique_ptr<char>{nullptr}),
                               "Attempts to constrain system",
                               "Input system was not found");
    }
    {
      sim.is_final = true;
      // no need to xor all_registered, because final() check is done first.
      // for a finaled() simulation, check the proper error
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_constrained(float{}),
                               "Attempts to constrain system",
                               "simulator has been finalized");
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_constrained(double{}),
                               "Attempts to constrain system",
                               "simulator has been finalized");
      CHECK_THROWS_NESTED_WITH(sim.check_if_system_can_be_constrained(
                                   std::unique_ptr<char>{nullptr}),
                               "Attempts to constrain system",
                               "simulator has been finalized");
    }
  }

  template <typename ConstraintSimulator>
  void check_constrain_interface(ConstraintSimulator& sim) {
    // do not have std::size_t as a system because it is the index!
    using TestSystems =
        tmpl::list<tmpl::type_<float>, tmpl::type_<char>, tmpl::type_<double>>;
    tmpl::for_each<TestSystems>([&, n_cons = 0UL](auto v) mutable {
      using system_type = tmpl::type_from<decltype(v)>;

      system_type sys{};
      (void)sim.constrain(sys);
      CHECK(sim.n_constraints() == ++n_cons);
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module.constrain_interface",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;

    {
      auto sim = mock::make_simulator<::mock::ConstraintsModule>(
          sim_config, arg_all_registered, is_final);
      // check if a constrainion object is returned
      using system_type = tmpl::type_<int>;
      system_type sys{};
      auto&& constraint = sim.constrain(sys);
      static_assert(
          cpp17::is_same_v<cpp20::remove_cvref_t<decltype(constraint)>,
                           Constraint<system_type, tmpl::list<>>>,
          "Constraint meta failure!");
    }

    {
      auto sim = mock::make_simulator<::mock::ConstraintsModule>(
          sim_config, arg_all_registered, is_final);
      check_constrain_interface(sim);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module.check_and_report_errors",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConstraintsModule>(
        sim_config, arg_all_registered, is_final);

    check_constrain_interface(sim);

    // there are many half-baked constraints here, so we see error reporting
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
} primitive_counter, rate_counter;  // a global variable in this scope

// define some constraints
// not as intense as Test_Constraint in Simulator/Constraints or
// Simulator/Interface/Constraints
namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintDefaulted) {
     public:
      ConstraintDefaulted() = default;
      ConstraintDefaulted(ConstraintDefaulted && other) noexcept
          : some_member(other.some_member){};
      ~ConstraintDefaulted() = default;
      // move constructor is deleted if a destructor is defined
      //      ~ConstraintDefaulted(){
      //        std::cout << "Destructor caleld!" << std::endl;
      //      }
      template <typename FirstSystem>
      inline void change_with_noop(FirstSystem & first_system) const {
        // see if first and second system can be changed
        first_system.n_indices += 1UL;
        first_system.n_indices -= 1UL;
      }

      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System & first_system,
                                std::size_t /* first_index */
      ) const {
        change_with_noop(first_system);
        ++primitive_counter;
      }
      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System & first_system,
                           std::size_t /* first_index */
      ) const {
        change_with_noop(first_system);
        ++rate_counter;
      }

      std::size_t some_member{23UL};  // not 0
    };

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithOneMember) {
     public:
      ConstraintWithOneMember(int a) : some_member(a){};
      ConstraintWithOneMember(ConstraintWithOneMember && other) noexcept
          : some_member(std::move(other.some_member)){};
      ~ConstraintWithOneMember() = default;
      template <typename FirstSystem>
      inline void change_with_noop(FirstSystem & first_system) const {
        // see if first and second system can be changed
        first_system.n_indices += 1UL;
        first_system.n_indices -= 1UL;
      }

      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System & first_system,
                                std::size_t /* first_index */
      ) const {
        change_with_noop(first_system);
        ++primitive_counter;
      }
      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System & first_system,
                           std::size_t /* first_index */
      ) const {
        change_with_noop(first_system);
        ++rate_counter;
      }

      int some_member;
    };

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithMember) {
     public:
      ConstraintWithMember(int i, char c) : int_member(i), char_member(c){};
      ConstraintWithMember(ConstraintWithMember && other) noexcept
          : int_member(other.int_member), char_member(other.char_member){};
      ~ConstraintWithMember() = default;

      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System& /*first_system*/,
                                std::size_t /* first_index */
      ) const {}
      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System& /*first_system*/,
                           std::size_t /* first_index */
      ) const {}
      explicit operator int() const noexcept { return int_member; }
      explicit operator char() const noexcept { return char_member; }

      int int_member;  // not 0
      char char_member;
    };

  }  // namespace constraints

}  // namespace elastica

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module.check_full_interface",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConstraintsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_constraints() == 0UL);

    using ::elastica::end;
    using ::elastica::constraints::ConstraintDefaulted;
    using ::elastica::interface::at;
    using ::mock::MockSystem;
    using ExampleConstraint = ConstraintDefaulted<>;

    std::size_t expected_count = 0UL;

    {
      using System = MockSystem;

      /// [constrain_setup]
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      System sys{100UL};
      std::string optional_constraint_name = "ExampleConstraint";
      // ExampleConstraint is some elastica::constraint satisfying the
      // constraint protocols
      /// [constrain_setup]

      /// [constrain_eg]
      // order of with() and at() don't matter
      sim.constrain(sys, optional_constraint_name)
          .with<ExampleConstraint>(/* ExampleConstraint arguments*/)
          .at(1UL, 2UL, 3UL);
      sim.constrain(sys, std::move(optional_constraint_name))
          .at(5UL, end - 3UL)
          .with<ExampleConstraint>(/* ExampleConstraint arguments*/);
      /// [constrain_eg]

      CHECK(sim.n_constraints() == 2UL);
      expected_count += 5UL;
    }
    {
      using System = MockSystem;

      System sys{100UL};
      std::string optional_constraint_name = "ExampleConstraint";

      /// [constrain_eg_no_curry]
      sim.constrain(sys, std::move(optional_constraint_name))
          .with(ExampleConstraint{}, at(1UL, 2UL, 5UL, 17UL));
      /// [constrain_eg_no_curry]

      CHECK(sim.n_constraints() == 3UL);
      expected_count += 4UL;
    }
    {
      using System = MockSystem;

      System sys{100UL};
      std::string optional_constraint_name = "ExampleConstraint";

      /// [constrain_eg_curry]
      // order of with() and at() don't matter
      sim.constrain(sys, optional_constraint_name)
          .with(ExampleConstraint{})
          .at(end - 2UL, end - 1UL);
      sim.constrain(sys, std::move(optional_constraint_name))
          .at(end - 50UL, 23UL, 64UL)
          .with(ExampleConstraint{});
      /// [constrain_eg_curry]

      CHECK(sim.n_constraints() == 5UL);
      expected_count += 5UL;
    }
    {
      using System = MockSystem;

      System sys{100UL};
      std::string optional_constraint_name = "ExampleConstraint";
      /// [constrain_eg_with_name]
      // order of with() and at() don't matter but put the with_name()
      // immediately after launching constrain()
      sim.constrain(sys)
          // .with_name((optional_constraint_name))
          .with(ExampleConstraint{})
          .at(end - 50UL, 32UL, 64UL);
      sim.constrain(sys)
          // .with_name(std::move(optional_constraint_name))
          .at(end - 50UL, 32UL, 64UL)
          .with(ExampleConstraint{});
      /// [constrain_eg_with_name]

      CHECK(sim.n_constraints() == 7UL);
      expected_count += 6UL;
    }

    // now check the finalize() method and check the number of constrainers
    {
      CHECK(primitive_counter.count == 0UL);
      CHECK(rate_counter.count == 0UL);

      // finalize
      CHECK_NOTHROW(sim.finalize());

      CHECK(primitive_counter.count == expected_count);
      CHECK(rate_counter.count == expected_count);
      std::size_t sum =
          std::accumulate(std::cbegin(sim.constrainer_batches()),
                          std::cend(sim.constrainer_batches()), 0UL,
                          [](std::size_t sum, auto const& batch) {
                            return sum + batch.size();
                          });
      CHECK(sum == 7UL);
      //      CHECK(sim.constrainers().size() == 7UL);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module.exception_for_finalize",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConstraintsModule>(
        sim_config, arg_all_registered, is_final);

    CHECK(sim.n_constraints() == 0UL);

    using ::elastica::constraints::ConstraintDefaulted;
    using ::mock::MockSystem;
    using ExampleConstraint = ConstraintDefaulted<>;

    {
      using System = MockSystem;
      System sys{100UL};
      // oops i forgot indices
      sim.constrain(sys).with<ExampleConstraint>();
      // oops i forgot indices
      sim.constrain(sys).with(ExampleConstraint{});
      // oops i forgot constraint
      sim.constrain(sys).at(6UL, 45UL);
      // WTF! This piece of code crashes the system due to RAM usage
      // I am HackerMan now by running elastica on clusters with this
      // code.
      //      // oops i forgot constrainion
      //      sim.constrain(fs, end - 5UL).to(ss, end - 20UL);
    }
    CHECK_THROWS_NESTED_WITH(sim.finalize(),
                             "Preparing the constraints module failed",
                             "Faults detected ", "listed by their");
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module.query",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConstraintsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_constraints() == 0UL);

    using ::elastica::end;
    using ::elastica::constraints::ConstraintDefaulted;
    using ::elastica::constraints::ConstraintWithOneMember;
    using ::elastica::interface::at;
    using ::elastica::interface::pair;
    using ::mock::MockSystem;
    using ExampleConstraint = ConstraintWithOneMember<>;
    {
      using System = MockSystem;

      // same setup as earlier
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      System sys{100UL};
      std::string optional_constraint_name = "MyConstraint";

      /// [query_eg]
      int argument = -2;
      auto my_constraint_key = sim.constrain(sys, optional_constraint_name)
                                   .with<ExampleConstraint>(argument)
                                   .at(end - 2UL, end - 1UL);

      sim.finalize();

      auto& my_constraint = sim.query(my_constraint_key);
      // you can now access member data of `my_constraint`
      /// [query_eg]

      CHECK(my_constraint.some_member == -2);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module.query_for_multiple",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConstraintsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_constraints() == 0UL);

    using ::elastica::end;
    using ::elastica::constraints::ConstraintWithMember;
    using ::elastica::constraints::ConstraintWithOneMember;
    using ::elastica::interface::at;
    using ::elastica::interface::pair;
    using ::mock::MockSystem;
    {
      using System = MockSystem;
      using ExampleConstraint = ConstraintWithOneMember<>;

      // same setup as earlier
      // is a physical system, e.g. CosseratRod, with 100 degrees of freedom
      System sys{100UL};
      std::string optional_constraint_name = "MyConstraint";
      auto first_constraint_key =
          sim.constrain(sys, optional_constraint_name)
              .with<ExampleConstraint>(56) /* ExampleConstraint arguments*/
              .at(end - 2UL, end - 1UL);
      auto second_constraint_key = sim.constrain(sys, optional_constraint_name)
                                       .with<ConstraintWithMember<>>(32, 'a')
                                       .at(1UL);
      auto third_constraint_key =
          sim.constrain(sys, optional_constraint_name)
              .with<ExampleConstraint>(-45) /* ExampleConstraint arguments*/
              .at(end - 2UL, end - 1UL);
      auto fourth_constraint_key = sim.constrain(sys, optional_constraint_name)
                                       .with<ConstraintWithMember<>>(42, 'd')
                                       .at(5UL);

      sim.finalize();
      auto& first_constraint = sim.query(first_constraint_key);
      auto& third_constraint = sim.query(third_constraint_key);

      CHECK(first_constraint.some_member == 56);
      CHECK(third_constraint.some_member == -45);

      auto& second_constraint = sim.query(second_constraint_key);
      auto& fourth_constraint = sim.query(fourth_constraint_key);
      CHECK(int(second_constraint) == 32);
      CHECK(char(second_constraint) == 'a');
      CHECK(int(fourth_constraint) == 42);
      CHECK(char(fourth_constraint) == 'd');
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Module.serialize",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    using namespace elastica::io;
    using ::elastica::constraints::ConstraintDefaulted;
    using ::mock::MockSystem;
    using ExampleConstraint = ConstraintDefaulted<>;
    using ::elastica::end;

    // no throw case
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConstraintsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConstraintsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of constraints
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.constrain(sys)                 //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleConstraint{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConstraintsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConstraintsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of constraints
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.constrain(sys)                 //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleConstraint{});
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
            DefaultConstraintsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConstraintsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of constraints
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.constrain(sys)                 //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleConstraint{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConstraintsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConstraintsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of constraints
          // added 5 less constraints!
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.constrain(sys)                 //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleConstraint{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_THROWS_NESTED_WITH(iarchive(sim), "Loading the Simulator state",
                                 "Number of constraints from");
      }
    }

    // throwing case, but this time the number of constraints are the same,
    // but some constraints are different
    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConstraintsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConstraintsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of constraints
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 10UL; ++i)
            sim.constrain(sys)                 //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleConstraint{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        auto sim_config = mock::make_simulator_configuration(
            DefaultConstraintsConfiguration{});
        // check cases without an error first.
        bool arg_all_registered = true;
        bool is_final = false;
        auto sim = mock::make_simulator<::mock::ConstraintsModule>(
            sim_config, arg_all_registered, is_final);

        {  // add a bunch of constraints
          // added 5 less constraints!
          using System = MockSystem;
          System sys{100UL};

          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.constrain(sys)                 //
                .at(10UL + i, end - 10UL - i)  //
                .with(ExampleConstraint{});

          struct AnotherConstraintTag {};
          using AnotherConstraint =
              tmpl::inherit<ExampleConstraint, AnotherConstraintTag>::type;
          for (std::size_t i = 0UL; i < 5UL; ++i)
            sim.constrain(sys)                 //
                .at(10UL + i, end - 10UL - i)  //
                .with(AnotherConstraint{});
        }

        CHECK_NOTHROW(sim.finalize());
        CHECK_THROWS_NESTED_WITH(iarchive(sim), "Loading the Simulator state",
                                 "Restored UUID", "does not match");
      }
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.Module.reset",
                     "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    auto sim_config =
        mock::make_simulator_configuration(DefaultConstraintsConfiguration{});
    // check cases without an error first.
    bool arg_all_registered = true;
    bool is_final = false;
    auto sim = mock::make_simulator<::mock::ConstraintsModule>(
        sim_config, arg_all_registered, is_final);
    CHECK(sim.n_constraints() == 0UL);

    using ::elastica::constraints::ConstraintWithMember;
    using ::mock::MockSystem;

    using System = MockSystem;
    using MemberConstraint = ConstraintWithMember<>;
    {
      System sys{100UL};
      std::string optional_constraint_name = "MyConstraints";

      for (std::size_t i = 0UL; i < 5UL; ++i)
        sim.constrain(sys, optional_constraint_name)
            .with<MemberConstraint>(32, 'a')
            .at(i);
    }

    sim.finalize();

    CHECK(sim.n_constraints() == 5UL);
    // CHECK(sim.ready()); we are not testing the mock simulator
    CHECK_FALSE(sim.constrainer_batches().empty());

    sim.reset(::elastica::Time{0.0});

    CHECK(sim.n_constraints() == 0UL);
    // CHECK_FALSE(sim.ready());  we are not testing the mock simulator
    CHECK(sim.constrainer_batches().empty());
  }

}  // namespace