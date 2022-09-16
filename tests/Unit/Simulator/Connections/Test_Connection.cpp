//
// Created by Tejaswin Parthasarathy on 1/23/21.
//

#include <array>
#include <deque>
#include <list>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Connections/Connection.hpp"
#include "Simulator/Connections/Protocols.hpp"
#include "Utilities/ProtocolHelpers.hpp"

// we have more tests in Interface/Test_Connection.cpp
// and additional tests in Connections/Test_MakeConnection.cpp
// we just give a examples and basic tests here

namespace elastica {

  namespace connections {

    template <typename Registrars, typename ConcreteConnection>
    struct TestConnection
        : elastica::connections::Connection<Registrars, ConcreteConnection> {
     public:
      using Parent =
          elastica::connections::Connection<Registrars, ConcreteConnection>;

      // functions
      using Parent::Parent;
      using Parent::operator=;
      using Parent::apply_forces_internal;
      using Parent::apply_torques_internal;

      // meta
      using Parent::derived_has_apply_forces;
      using Parent::derived_has_apply_torques;
      using Parent::derived_has_no_apply_forces;
      using Parent::derived_has_no_apply_torques;

      using Parent::derived_has_deferred_apply_forces;
      using Parent::derived_has_deferred_apply_torques;
      using Parent::derived_has_no_deferred_apply_forces;
      using Parent::derived_has_no_deferred_apply_torques;
    };

  }  // namespace connections

}  // namespace elastica

#define ELASTICA_DEFINE_CONNECTION(class_name)                          \
  template <typename Registrars>                                        \
  class class_name;                                                     \
                                                                        \
  namespace Registrars {                                                \
    using class_name =                                                  \
        ::Registration::Registrar<::elastica::connections::class_name>; \
  }                                                                     \
                                                                        \
  template <typename Registrars = tmpl::list<Registrars::class_name>>   \
  class class_name : public TestConnection<Registrars, class_name<Registrars>>
//******************************************************************************

/// [cwam]
namespace elastica {
  namespace connections {
    ELASTICA_DEFINE_CONNECTION(ConnectionExample) {
     public:
      explicit ConnectionExample(int& i) noexcept : i_(i){};
      ConnectionExample(ConnectionExample && other) noexcept : i_(other.i_) {}
      ~ConnectionExample() override = default;
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /*time*/, FirstSystem& /*first_system*/,
                        std::size_t /* first_index */,
                        SecondSystem& /*second_system*/,
                        std::size_t /* second_index */) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /*time*/, FirstSystem& /*first_system*/,
                         std::size_t /* first_index */,
                         SecondSystem& /*second_system*/,
                         std::size_t /* second_index */) const {
        // do something
        --i_;
      }
      static std::string name() noexcept { return "ConstrainExample"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace connections

}  // namespace elastica

/// [cwam]

/// [cwfo]

namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithForcesOnly) {
     public:
      explicit ConnectionWithForcesOnly(int& f) noexcept : i_(f){};
      ConnectionWithForcesOnly(ConnectionWithForcesOnly && other) noexcept
          : i_(other.i_) {}
      ~ConnectionWithForcesOnly() override = default;
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /*time*/, FirstSystem& /*first_system*/,
                        std::size_t /* first_index */,
                        SecondSystem& /*second_system*/,
                        std::size_t /* second_index */) const {
        // do something
        ++i_;
      }
      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept { return "ConnectionWithForcesOnly"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace connections

}  // namespace elastica

/// [cwfo]

/// [cwto]

namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithTorquesOnly) {
     public:
      explicit ConnectionWithTorquesOnly(int& f) noexcept : i_(f){};
      ConnectionWithTorquesOnly(ConnectionWithTorquesOnly && other) noexcept
          : i_(other.i_) {}
      ~ConnectionWithTorquesOnly() override = default;
      // no constrain values automatically assumed to be no-op
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /*time*/, FirstSystem& /*first_system*/,
                         std::size_t /* first_index */,
                         SecondSystem& /*second_system*/,
                         std::size_t /* second_index */) const {
        // do something
        --i_;
      }
      static std::string name() noexcept { return "ConnectionWithTorquesOnly"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace connections

}  // namespace elastica

/// [cwto]

namespace {
  using namespace elastica::connections;
  using namespace elastica;

  template <typename Conn, typename... Args>
  constexpr auto check_and_make_connection(Args&&... args) {
    static_assert(::tt::assert_conforms_to<Conn, protocols::Connection>,
                  "Connection protocol violation!");
    return Conn(std::forward<Args>(args)...);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Connections.Connection.with_all_members",
                     "[Unit][Simulator][Connections]") {
    int i = 2;
    using Conn = ::elastica::connections::ConnectionExample<>;
    auto my_connection(check_and_make_connection<Conn>(i));

    int mock_time = 3;
    int mock_first_system = 2;
    std::size_t mock_first_index = 1UL;
    int mock_second_system = 2;
    std::size_t mock_second_index = 3UL;

    // check-metaprogramming
    {
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_first_system),
                                         decltype(mock_second_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_first_system),
                                          decltype(mock_second_system)>(),
          "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_first_system),
                        decltype(mock_second_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_first_system),
                        decltype(mock_second_system)>(),
                    "connection metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                                 mock_second_system, mock_second_index);
      CHECK(my_connection.i_ == 3);
      my_connection.apply_torques(mock_time, mock_first_system,
                                  mock_first_index, mock_second_system,
                                  mock_second_index);
      CHECK(my_connection.i_ == 2);
    };

    auto check_public_interface = [&]() {
      my_connection.apply_forces_internal(mock_time, mock_first_system,
                                          mock_first_index, mock_second_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == 3);
      my_connection.apply_torques_internal(mock_time, mock_first_system,
                                           mock_first_index, mock_second_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == 2);
    };

    check_public_members();
    check_public_interface();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Connections.Connection.with_forces_only",
                     "[Unit][Simulator][Connections]") {
    int i = 2;
    using Conn = ::elastica::connections::ConnectionWithForcesOnly<>;
    auto my_connection(check_and_make_connection<Conn>(i));

    int mock_time = 3;
    int mock_first_system = 2;
    std::size_t mock_first_index = 1UL;
    int mock_second_system = 2;
    std::size_t mock_second_index = 3UL;

    // check-metaprogramming
    {
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_first_system),
                                         decltype(mock_second_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_first_system),
                                             decltype(mock_second_system)>(),
          "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_first_system),
                        decltype(mock_second_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_first_system),
                        decltype(mock_second_system)>(),
                    "connection metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                                 mock_second_system, mock_second_index);
      CHECK(my_connection.i_ == (2 + 1));
    };

    auto check_public_interface = [&]() {
      my_connection.apply_forces_internal(mock_time, mock_first_system,
                                          mock_first_index, mock_second_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == (2 + 1 + 1));

      my_connection.apply_torques_internal(mock_time, mock_first_system,
                                           mock_first_index, mock_second_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == (2 + 1 + 1));
    };

    check_public_members();
    check_public_interface();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Connections.Connection.with_torques_only",
                     "[Unit][Simulator][Connections]") {
    int i = 10;
    using Conn = ::elastica::connections::ConnectionWithTorquesOnly<>;
    auto my_connection(check_and_make_connection<Conn>(i));

    int mock_time = 3;
    int mock_first_system = 2;
    std::size_t mock_first_index = 1UL;
    int mock_second_system = 2;
    std::size_t mock_second_index = 3UL;

    // check-metaprogramming
    {
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_first_system),
                                            decltype(mock_second_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_first_system),
                                          decltype(mock_second_system)>(),
          "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_first_system),
                        decltype(mock_second_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_first_system),
                        decltype(mock_second_system)>(),
                    "connection metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      // check the public memebr
      my_connection.apply_torques(mock_time, mock_first_system,
                                  mock_first_index, mock_second_system,
                                  mock_second_index);
      CHECK(my_connection.i_ == (10 - 1));
    };

    auto check_public_interface = [&]() {
      my_connection.apply_forces_internal(mock_time, mock_first_system,
                                          mock_first_index, mock_second_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == (10 - 1));

      my_connection.apply_torques_internal(mock_time, mock_first_system,
                                           mock_first_index, mock_second_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == (10 - 1 - 1));
    };

    check_public_members();
    check_public_interface();
  }

}  // namespace

/// [cwsf]
namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithForcesForSpecificSystem) {
     public:
      explicit ConnectionWithForcesForSpecificSystem(int& i) noexcept : i_(i){};
      ConnectionWithForcesForSpecificSystem(
          ConnectionWithForcesForSpecificSystem && other) noexcept
          : i_(other.i_) {}
      ~ConnectionWithForcesForSpecificSystem() override = default;

      // overloaded for specialized systems

      // 1. an integer specialization
      template <typename TimeType, typename SecondSystem>
      void apply_forces(TimeType /*time*/, int& /*first_system*/,
                        std::size_t /* first_index */,
                        SecondSystem& /*second_system*/,
                        std::size_t /* second_index */) const {
        // do something
        ++i_;
      }

      // Usually, you would need the following method (2) too, with order of
      // arguments switched. Such a method should delegate to the above member
      // function since the algorithm should be independent of the ordering of
      // arguments, and depend only on their types
      // But because \elastica's detection mechanism is smart, you do not need
      // such a function and the above member function works for pairs of type
      // (int -- SecondSystem) or (SecondSystem -- int)!
      /*
      template <typename TimeType, typename FirstSystem>
      void apply_forces(TimeType time, FirstSystem & first_system,
                        std::size_t first_index, int& second_system, std::size_t
      second_index) const {
        // do something, usually delegate to the other member
        apply_forces(time, second_system, second_index, first_system,
                     first_index);
      }
       */

      // 3. Specialization when the first system is a float
      template <typename TimeType, typename SecondSystem>
      void apply_forces(TimeType /*time*/, float& /*first_system*/,
                        std::size_t /* first_index */,
                        SecondSystem& /*second_system*/,
                        std::size_t /* second_index */) const {
        // do something
        --i_;
      }

      // Similar to how (2) is unneeded for (1), you don't need the following
      // member function (4), the switched counterpart to (3)
      /*
      template <typename TimeType, typename FirstSystem>
      void apply_forces(TimeType time, FirstSystem & first_system,
                        std::size_t first_index, float& second_system,
      std::size_t second_index) const {
        // do something, usually delegate to the other member
        apply_forces(time, second_system, second_index, first_system,
                     first_index);
      }
       */

      // An added benefit to skipping the members (2) and (4) is when a pair of
      // systems  of type (int--float) gets called. If we had (2) and (4)
      // specified the function call to an (int--float) pair is likely to be
      // ambiguous since both (1) and (4) or (2) and (3) match. Now that we have
      // skipped (2) and (4), there is no conflict here

      // 5. maximal specialization when the first system is a float and the
      // second system is bool. Also works for first system bool and second
      // system float
      template <typename TimeType>
      void apply_forces(TimeType /*time*/, float& /*first_system*/,
                        std::size_t /* first_index */, bool& /*second_system*/,
                        std::size_t /* second_index */) const {
        // do something
        i_ *= 2;
      }

      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConnectionWithForcesForSpecificSystem";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace connections

}  // namespace elastica
/// [cwsf]

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Connections.Connection.with_forces_for_specific_system",
      "[Unit][Simulator][Connections]") {
    int i = 2;
    using Conn =
        ::elastica::connections::ConnectionWithForcesForSpecificSystem<>;
    auto my_connection(check_and_make_connection<Conn>(i));

    int mock_time = 3;
    std::size_t mock_first_index = 1UL;
    std::size_t mock_second_index = 3UL;

    int mock_int_system = 2;
    float mock_float_system = 2.0F;

    char mock_other_system = 'a';

    // check-metaprogramming
    {
      // int
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_int_system),
                                         decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_other_system),
                                            decltype(mock_int_system)>(),
          "connection metaprogramming failure!");

      // float
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_float_system),
                                         decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_other_system),
                                            decltype(mock_float_system)>(),
          "connection metaprogramming failure!");

      // float + int specialization
      static_assert(Conn::derived_has_apply_forces<decltype(mock_time),
                                                   decltype(mock_float_system),
                                                   decltype(mock_int_system)>(),
                    "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_int_system),
                                         decltype(mock_float_system)>(),
          "connection metaprogramming failure!");

      // negation
      static_assert(Conn::derived_has_no_apply_forces<decltype(mock_time),
                                                      double, double>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_apply_forces<decltype(mock_time), char,
                                                      double>(),
                    "connection metaprogramming failure!");

      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_int_system),
                                             decltype(mock_float_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_float_system),
                                             decltype(mock_int_system)>(),
          "connection metaprogramming failure!");

      // deferred
      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_float_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_float_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_int_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_int_system)>(),
                    "connection metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_connection.apply_forces(mock_time, mock_int_system, mock_first_index,
                                 mock_other_system, mock_second_index);
      // doesnt compile, as expected, use apply_forces_internal instead
      /*
      my_connection.apply_forces(mock_time, mock_other_system, mock_first_index,
                                 mock_int_system, mock_second_index);
      */
      //
      my_connection.apply_forces(mock_time, mock_int_system, mock_first_index,
                                 mock_other_system, mock_second_index);
      const int expected1 = (2 + 2);
      CHECK(my_connection.i_ == expected1);

      my_connection.apply_forces(mock_time, mock_float_system, mock_first_index,
                                 mock_other_system, mock_second_index);
      // doesnt compile, as expected, use apply_forces_internal instead
      /*
      my_connection.apply_forces(mock_time, mock_other_system, mock_first_index,
                                 mock_float_system, mock_second_index);
      */
      //
      my_connection.apply_forces(mock_time, mock_float_system, mock_first_index,
                                 mock_other_system, mock_second_index);
      const int expected2 = expected1 - 2;
      CHECK(my_connection.i_ == expected2);

      // If done incorrectly, this is an ambiguous call with int, float
      // Since we provide an overload resolution mechanism in the base class,
      // we can have this around
      my_connection.apply_forces(mock_time, mock_float_system, mock_first_index,
                                 mock_int_system, mock_second_index);
      CHECK(my_connection.i_ == expected2 - 1);
      my_connection.apply_forces(mock_time, mock_int_system, mock_first_index,
                                 mock_float_system, mock_second_index);
      CHECK(my_connection.i_ == expected2);
    };

    auto check_public_interface = [&]() {
      const int expected = my_connection.i_;
      my_connection.apply_forces_internal(mock_time, mock_int_system,
                                          mock_first_index, mock_other_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_first_index, mock_int_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == (expected + 2));

      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_first_index, mock_other_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_first_index, mock_float_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_first_index, mock_int_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_int_system,
                                          mock_first_index, mock_float_system,
                                          mock_second_index);
      const int expected2 = expected;
      CHECK(my_connection.i_ == expected2);

      // no-operation
      my_connection.apply_torques_internal(mock_time, mock_int_system,
                                           mock_first_index, mock_other_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_first_index, mock_int_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected2);
      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_first_index, mock_other_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_first_index, mock_float_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected2);
      my_connection.apply_torques_internal(mock_time, mock_int_system,
                                           mock_first_index, mock_float_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_first_index, mock_int_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected2);
    };

    // check constrain values for the above cases
    auto check_interface_is_noop_for_nonoverloaded_types = [&]() {
      const int expected = my_connection.i_;
      double mock_double_system(5.0);
      char mock_char_system('a');
      my_connection.apply_forces_internal(mock_time, mock_double_system,
                                          mock_first_index, mock_char_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_char_system,
                                          mock_first_index, mock_double_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_double_system,
                                           mock_first_index, mock_char_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_char_system,
                                           mock_first_index, mock_double_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);
    };

    auto check_interface_for_fully_specialized_types = [&]() {
      const int expected = my_connection.i_;
      float mock_float_system(5.0F);
      bool mock_bool_system(true);

      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_first_index, mock_bool_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      const int expected2 = 2 * expected;
      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_first_index, mock_bool_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected2);

      const int expected3 = 2 * expected2;
      my_connection.apply_forces_internal(mock_time, mock_bool_system,
                                          mock_first_index, mock_float_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected3);
    };

    check_public_members();
    check_public_interface();
    check_interface_is_noop_for_nonoverloaded_types();
    check_interface_for_fully_specialized_types();
  }
}  // namespace

/// [cwtf]
namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithForcesForTemplatedSystems) {
     public:
      explicit ConnectionWithForcesForTemplatedSystems(int& i) noexcept
          : i_(i){};
      ConnectionWithForcesForTemplatedSystems(
          ConnectionWithForcesForTemplatedSystems && other) noexcept
          : i_(other.i_) {}
      ~ConnectionWithForcesForTemplatedSystems() override = default;
      // overloaded for specialized systems

      // 1. a vector type as first system
      template <typename TimeType, typename T, typename System>
      void apply_forces(TimeType /*time*/, std::vector<T>& /*first_system*/,
                        std::size_t /* first_index */,
                        System& /* second_system*/,
                        std::size_t /* second_index */) const {
        // do something
        ++i_;
      }

      // 2. because \elastica's overload resolution is smart you can skip this
      /*
      template <typename TimeType, typename T, typename System>
      void apply_forces(TimeType time, System & first_system, std::size_t
      first_index, std::vector<T>& second_system, std::size_t second_index)
      const {
        // do something
        apply_forces(time, second_system, second_index, first_system,
                     first_index);
      }
      */

      // 3. a list type as first system
      template <typename TimeType, typename T, typename System>
      void apply_forces(TimeType /*time*/, std::list<T>& /*first_system*/,
                        std::size_t /* first_index */,
                        System& /* second_system*/,
                        std::size_t /* second_index */) const {
        // do something
        --i_;
      }

      // 4. because \elastica's overload resolution is smart you can skip this
      /*
      template <typename TimeType, typename T, typename System>
      void apply_forces(TimeType time, System & first_system, std::size_t
      first_index, std::list<T>& second_system, std::size_t second_index) const
      {
        // do something
        apply_forces(time, second_system, second_index, first_system,
                     first_index);
      }
       */

      // 5. a "full" specialization with vector as first and list as second type
      template <typename TimeType, typename T, typename U>
      void apply_forces(TimeType /*time*/, std::vector<T>& /*first_system*/,
                        std::size_t /* first_index */,
                        std::list<U>& /* second_system*/,
                        std::size_t /* second_index */) const {
        // do something
        i_ *= 2;
      }

      // 6. For a full specialization in this case, you have to provide this
      // member.
      // The reason is that if you do not provide this
      // member, the <list, system> specialization (3) is better qualified as it
      // has been specified. In any case, this scenario is not a valid
      // connection, since a <list, system> specialization (3) should be
      // equivalent to <list, vector> specialization (6) since
      // since vector is also a valid system
      template <typename TimeType, typename T, typename U>
      void apply_forces(TimeType time, std::list<T> & first_system,
                        std::size_t first_index, std::vector<U> & second_system,
                        std::size_t second_index) const {
        // do something
        apply_forces(time, second_system, second_index, first_system,
                     first_index);
      }

      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConnectionWithForcesForTemplatedSystems";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace connections

}  // namespace elastica
/// [cwtf]

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Connections.Connection.with_forces_for_templated_system",
      "[Unit][Simulator][Connections]") {
    int i = 2;
    using Conn =
        ::elastica::connections::ConnectionWithForcesForTemplatedSystems<>;
    auto my_connection(check_and_make_connection<Conn>(i));

    std::vector<int> mock_vector_system{1, 2, 3};
    std::list<int> mock_list_system = {4, 5, 6};

    int mock_time = 3;
    std::size_t mock_first_index = 1UL;
    std::size_t mock_second_index = 3UL;

    auto mock_other_system = std::deque<int>{1, 2, 3};

    // check-metaprogramming
    {
      // vector
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_vector_system),
                                         decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_other_system),
                                            decltype(mock_vector_system)>(),
          "connection metaprogramming failure!");

      // list
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_list_system),
                                         decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_other_system),
                                            decltype(mock_list_system)>(),
          "connection metaprogramming failure!");

      // float + int specialization
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_list_system),
                                         decltype(mock_vector_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_apply_forces<decltype(mock_time),
                                         decltype(mock_vector_system),
                                         decltype(mock_list_system)>(),
          "connection metaprogramming failure!");

      // negation
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            std::deque<double>, std::string>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            std::deque<double>, std::string>(),
          "connection metaprogramming failure!");

      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_vector_system),
                                             decltype(mock_list_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_list_system),
                                             decltype(mock_vector_system)>(),
          "connection metaprogramming failure!");

      // deferred
      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_list_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_list_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_list_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_list_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_list_system),
                        decltype(mock_vector_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_list_system),
                        decltype(mock_vector_system)>(),
                    "connection metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_connection.apply_forces(mock_time, mock_vector_system,
                                 mock_first_index, mock_other_system,
                                 mock_second_index);
      // doesnt compile, as expected, use apply_forces_internal instead
      /*
      my_connection.apply_forces(mock_time, mock_other_system, mock_first_index,
                                 mock_vector_system, mock_second_index);
      */
      my_connection.apply_forces(mock_time, mock_vector_system,
                                 mock_first_index, mock_other_system,
                                 mock_second_index);
      const int expected1 = (2 + 2);
      CHECK(my_connection.i_ == expected1);

      my_connection.apply_forces(mock_time, mock_list_system, mock_first_index,
                                 mock_other_system, mock_second_index);
      // doesnt compile, as expected, use apply_forces_internal instead
      /*
      my_connection.apply_forces(mock_time, mock_other_system, mock_first_index,
                                 mock_list_system, mock_second_index);
      */
      my_connection.apply_forces(mock_time, mock_list_system, mock_first_index,
                                 mock_other_system, mock_second_index);
      const int expected2 = expected1 - 2;
      CHECK(my_connection.i_ == expected2);

      // this is an ambiguous call without \elastica's overload resolution
      // mechanism
      // this should call this specialization of list<>, System
      my_connection.apply_forces(mock_time, mock_list_system, mock_first_index,
                                 mock_vector_system, mock_second_index);
      const int expected3 = 2 * expected2;
      CHECK(my_connection.i_ == expected3);

      // this should call this specialization of vector<>, list<>
      my_connection.apply_forces(mock_time, mock_vector_system,
                                 mock_first_index, mock_list_system,
                                 mock_second_index);
      CHECK(my_connection.i_ == 2 * expected3);
    };

    auto check_public_interface = [&]() {
      const int expected = my_connection.i_;
      my_connection.apply_forces_internal(mock_time, mock_vector_system,
                                          mock_first_index, mock_other_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_first_index, mock_vector_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == (expected + 2));

      my_connection.apply_forces_internal(mock_time, mock_list_system,
                                          mock_first_index, mock_other_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_first_index, mock_list_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      // sadly this seems to call lhe list, System specialization, not the
      // list, vector specialization
      my_connection.apply_forces_internal(mock_time, mock_list_system,
                                          mock_first_index, mock_vector_system,
                                          mock_second_index);

      my_connection.apply_forces_internal(mock_time, mock_vector_system,
                                          mock_first_index, mock_list_system,
                                          mock_second_index);
      const int expected2 = 2 * 2 * expected;
      CHECK(my_connection.i_ == expected2);

      // no-operation
      my_connection.apply_torques_internal(mock_time, mock_vector_system,
                                           mock_first_index, mock_other_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_first_index, mock_vector_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected2);
      my_connection.apply_torques_internal(mock_time, mock_list_system,
                                           mock_first_index, mock_other_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_first_index, mock_list_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected2);
      my_connection.apply_torques_internal(mock_time, mock_vector_system,
                                           mock_first_index, mock_list_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_list_system,
                                           mock_first_index, mock_vector_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected2);
    };

    // check constrain values for the above cases
    auto check_members_are_noop_for_nonoverloaded_types = [&]() {
      const int expected = my_connection.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      double mock_double_system(5.0);
      char mock_char_system('a');
      my_connection.apply_forces_internal(mock_time, mock_double_system,
                                          mock_first_index, mock_char_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_char_system,
                                          mock_first_index, mock_double_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      // check a template
      my_connection.apply_forces_internal(mock_time, mock_array_system,
                                          mock_first_index, mock_char_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_char_system,
                                          mock_first_index, mock_array_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_double_system,
                                           mock_first_index, mock_char_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_char_system,
                                           mock_first_index, mock_double_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);
    };

    check_public_members();
    check_public_interface();
    check_members_are_noop_for_nonoverloaded_types();
  }
}  // namespace

/// [cwst]
namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithTorquesForSpecificSystem) {
     public:
      explicit ConnectionWithTorquesForSpecificSystem(int& i) noexcept
          : i_(i){};
      ConnectionWithTorquesForSpecificSystem(
          ConnectionWithTorquesForSpecificSystem && other) noexcept
          : i_(other.i_) {}
      ~ConnectionWithTorquesForSpecificSystem() override = default;

      // overloaded for specialized systems

      // 1. an integer specialization
      template <typename TimeType, typename SecondSystem>
      void apply_torques(TimeType /*time*/, int& /*first_system*/,
                         std::size_t /* first_index */,
                         SecondSystem& /*second_system*/,
                         std::size_t /* second_index */) const {
        // do something
        ++i_;
      }

      // Usually, you would need the following method (2) too, with order of
      // arguments switched. Such a method should delegate to the above member
      // function since the algorithm should be independent of the ordering of
      // arguments, and depend only on their types
      // But because \elastica's detection mechanism is smart, you do not need
      // such a function and the above member function works for pairs of type
      // (int -- SecondSystem) or (SecondSystem -- int)!
      /*
      template <typename TimeType, typename FirstSystem>
      void apply_torques(TimeType time, FirstSystem & first_system,
                        std::size_t first_index, int& second_system, std::size_t
      second_index) const {
        // do something, usually delegate to the other member
        apply_torques(time, second_system, second_index, first_system,
                     first_index);
      }
       */

      // 3. Specialization when the first system is a float
      template <typename TimeType, typename SecondSystem>
      void apply_torques(TimeType /*time*/, float& /*first_system*/,
                         std::size_t /* first_index */,
                         SecondSystem& /*second_system*/,
                         std::size_t /* second_index */) const {
        // do something
        --i_;
      }

      // Similar to how (2) is unneeded for (1), you don't need the following
      // member function (4), the switched counterpart to (3)
      /*
      template <typename TimeType, typename FirstSystem>
      void apply_torques(TimeType time, FirstSystem & first_system,
                        std::size_t first_index, float& second_system,
      std::size_t second_index) const {
        // do something, usually delegate to the other member
        apply_torques(time, second_system, second_index, first_system,
                     first_index);
      }
       */

      // An added benefit to skipping the members (2) and (4) is when a pair of
      // systems  of type (int--float) gets called. If we had (2) and (4)
      // specified the function call to an (int--float) pair is likely to be
      // ambiguous since both (1) and (4) or (2) and (3) match. Now that we have
      // skipped (2) and (4), there is no conflict here

      // 5. maximal specialization when the first system is a float and the
      // second system is bool. Also works for first system bool and second
      // system float
      template <typename TimeType>
      void apply_torques(TimeType /*time*/, float& /*first_system*/,
                         std::size_t /* first_index */, bool& /*second_system*/,
                         std::size_t /* second_index */) const {
        // do something
        i_ *= 2;
      }

      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConnectionWithTorquesForSpecificSystem";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace connections

}  // namespace elastica
/// [cwst]

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Connections.Connection.with_torques_for_specific_system",
      "[Unit][Simulator][Connections]") {
    int i = 2;
    using Conn =
        ::elastica::connections::ConnectionWithTorquesForSpecificSystem<>;
    auto my_connection(check_and_make_connection<Conn>(i));

    int mock_time = 3;
    std::size_t mock_first_index = 1;
    std::size_t mock_second_index = 3;

    int mock_int_system = 2;
    float mock_float_system = 2.0F;

    char mock_other_system = 'a';

    // check-metaprogramming
    {
      // int
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_int_system),
                                          decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_other_system),
                                             decltype(mock_int_system)>(),
          "connection metaprogramming failure!");

      // float
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_float_system),
                                          decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_other_system),
                                             decltype(mock_float_system)>(),
          "connection metaprogramming failure!");

      // float + int specialization
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_float_system),
                                          decltype(mock_int_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_int_system),
                                          decltype(mock_float_system)>(),
          "connection metaprogramming failure!");

      // negation
      static_assert(Conn::derived_has_no_apply_torques<decltype(mock_time),
                                                       double, double>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_apply_torques<decltype(mock_time),
                                                       char, double>(),
                    "connection metaprogramming failure!");

      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_int_system),
                                            decltype(mock_float_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_float_system),
                                            decltype(mock_int_system)>(),
          "connection metaprogramming failure!");

      // deferred
      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_float_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_float_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_int_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_int_system)>(),
                    "connection metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_connection.apply_torques(mock_time, mock_int_system, mock_first_index,
                                  mock_other_system, mock_second_index);
      // doesnt compile, as expected, use apply_torques_internal instead
      /*
      my_connection.apply_torques(mock_time, mock_other_system,
      mock_first_index, mock_int_system, mock_second_index);
      */
      //
      my_connection.apply_torques(mock_time, mock_int_system, mock_first_index,
                                  mock_other_system, mock_second_index);
      const int expected1 = (2 + 2);
      CHECK(my_connection.i_ == expected1);

      my_connection.apply_torques(mock_time, mock_float_system,
                                  mock_first_index, mock_other_system,
                                  mock_second_index);
      // doesnt compile, as expected, use apply_torques_internal instead
      /*
      my_connection.apply_torques(mock_time, mock_other_system,
      mock_first_index, mock_float_system, mock_second_index);
      */
      //
      my_connection.apply_torques(mock_time, mock_float_system,
                                  mock_first_index, mock_other_system,
                                  mock_second_index);
      const int expected2 = expected1 - 2;
      CHECK(my_connection.i_ == expected2);

      // If done incorrectly, this is an ambiguous call with int, float
      // Since we provide an overload resolution mechanism in the base class,
      // we can have this around
      my_connection.apply_torques(mock_time, mock_float_system,
                                  mock_first_index, mock_int_system,
                                  mock_second_index);
      CHECK(my_connection.i_ == expected2 - 1);
      my_connection.apply_torques(mock_time, mock_int_system, mock_first_index,
                                  mock_float_system, mock_second_index);
      CHECK(my_connection.i_ == expected2);
    };

    auto check_public_interface = [&]() {
      const int expected = my_connection.i_;
      my_connection.apply_torques_internal(mock_time, mock_int_system,
                                           mock_first_index, mock_other_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_first_index, mock_int_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == (expected + 2));

      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_first_index, mock_other_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_first_index, mock_float_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_first_index, mock_int_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_int_system,
                                           mock_first_index, mock_float_system,
                                           mock_second_index);
      const int expected2 = expected;
      CHECK(my_connection.i_ == expected2);

      // no-operation
      my_connection.apply_forces_internal(mock_time, mock_int_system,
                                          mock_first_index, mock_other_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_first_index, mock_int_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected2);
      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_first_index, mock_other_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_first_index, mock_float_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected2);
      my_connection.apply_forces_internal(mock_time, mock_int_system,
                                          mock_first_index, mock_float_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_first_index, mock_int_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected2);
    };

    // check constrain values for the above cases
    auto check_interface_is_noop_for_nonoverloaded_types = [&]() {
      const int expected = my_connection.i_;
      double mock_double_system(5.0);
      char mock_char_system('a');
      my_connection.apply_torques_internal(mock_time, mock_double_system,
                                           mock_first_index, mock_char_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_char_system,
                                           mock_first_index, mock_double_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_double_system,
                                          mock_first_index, mock_char_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_char_system,
                                          mock_first_index, mock_double_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);
    };

    auto check_interface_for_fully_specialized_types = [&]() {
      const int expected = my_connection.i_;
      float mock_float_system(5.0F);
      bool mock_bool_system(true);

      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_first_index, mock_bool_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      const int expected2 = 2 * expected;
      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_first_index, mock_bool_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected2);

      const int expected3 = 2 * expected2;
      my_connection.apply_torques_internal(mock_time, mock_bool_system,
                                           mock_first_index, mock_float_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected3);
    };

    check_public_members();
    check_public_interface();
    check_interface_is_noop_for_nonoverloaded_types();
    check_interface_for_fully_specialized_types();
  }
}  // namespace

/// [cwtt]
namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithTorquesForTemplatedSystems) {
     public:
      explicit ConnectionWithTorquesForTemplatedSystems(int& i) noexcept
          : i_(i){};
      ConnectionWithTorquesForTemplatedSystems(
          ConnectionWithTorquesForTemplatedSystems && other) noexcept
          : i_(other.i_) {}
      ~ConnectionWithTorquesForTemplatedSystems() override = default;
      // overloaded for specialized systems

      // 1. a vector type as first system
      template <typename TimeType, typename T, typename System>
      void apply_torques(TimeType /*time*/, std::vector<T>& /*first_system*/,
                         std::size_t /* first_index */,
                         System& /* second_system*/,
                         std::size_t /* second_index */) const {
        // do something
        ++i_;
      }

      // 2. because \elastica's overload resolution is smart you can skip this
      /*
      template <typename TimeType, typename T, typename System>
      void apply_torques(TimeType time, System & first_system, std::size_t
      first_index, std::vector<T>& second_system, std::size_t second_index)
      const {
        // do something
        apply_torques(time, second_system, second_index, first_system,
                     first_index);
      }
      */

      // 3. a list type as first system
      template <typename TimeType, typename T, typename System>
      void apply_torques(TimeType /*time*/, std::list<T>& /*first_system*/,
                         std::size_t /* first_index */,
                         System& /* second_system*/,
                         std::size_t /* second_index */) const {
        // do something
        --i_;
      }

      // 4. because \elastica's overload resolution is smart you can skip this
      /*
      template <typename TimeType, typename T, typename System>
      void apply_torques(TimeType time, System & first_system, std::size_t
      first_index, std::list<T>& second_system, std::size_t second_index) const
      {
        // do something
        apply_torques(time, second_system, second_index, first_system,
                     first_index);
      }
       */

      // 5. a "full" specialization with vector as first and list as second type
      template <typename TimeType, typename T, typename U>
      void apply_torques(TimeType /*time*/, std::vector<T>& /*first_system*/,
                         std::size_t /* first_index */,
                         std::list<U>& /* second_system*/,
                         std::size_t /* second_index */) const {
        // do something
        i_ *= 2;
      }

      // 6. For a full specialization in this case, you have to provide this
      // member.
      // The reason is that if you do not provide this
      // member, the <list, system> specialization (3) is better qualified as it
      // has been specified. In any case, this scenario is not a valid
      // connection, since a <list, system> specialization (3) should be
      // equivalent to <list, vector> specialization (6) since
      // since vector is also a valid system
      template <typename TimeType, typename T, typename U>
      void apply_torques(
          TimeType time, std::list<T> & first_system, std::size_t first_index,
          std::vector<U> & second_system, std::size_t second_index) const {
        // do something
        apply_torques(time, second_system, second_index, first_system,
                      first_index);
      }

      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConnectionWithTorquesForTemplatedSystems";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace connections

}  // namespace elastica
/// [cwtt]

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Connections.Connection.with_torques_for_templated_system",
      "[Unit][Simulator][Connections]") {
    int i = 2;
    using Conn =
        ::elastica::connections::ConnectionWithTorquesForTemplatedSystems<>;
    auto my_connection(check_and_make_connection<Conn>(i));

    std::vector<int> mock_vector_system{1, 2, 3};
    std::list<int> mock_list_system = {4, 5, 6};

    int mock_time = 3;
    std::size_t mock_first_index = 1UL;
    std::size_t mock_second_index = 3UL;

    auto mock_other_system = std::deque<int>{1, 2, 3};

    // check-metaprogramming
    {
      // vector
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_vector_system),
                                          decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_other_system),
                                             decltype(mock_vector_system)>(),
          "connection metaprogramming failure!");

      // list
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_list_system),
                                          decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_other_system),
                                             decltype(mock_list_system)>(),
          "connection metaprogramming failure!");

      // float + int specialization
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_list_system),
                                          decltype(mock_vector_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_apply_torques<decltype(mock_time),
                                          decltype(mock_vector_system),
                                          decltype(mock_list_system)>(),
          "connection metaprogramming failure!");

      // negation
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             std::deque<double>, std::string>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             std::deque<double>, std::string>(),
          "connection metaprogramming failure!");

      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_vector_system),
                                            decltype(mock_list_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_list_system),
                                            decltype(mock_vector_system)>(),
          "connection metaprogramming failure!");

      // deferred
      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_list_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_list_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_list_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_list_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_list_system),
                        decltype(mock_vector_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_list_system),
                        decltype(mock_vector_system)>(),
                    "connection metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_connection.apply_torques(mock_time, mock_vector_system,
                                  mock_first_index, mock_other_system,
                                  mock_second_index);
      // doesnt compile, as expected, use apply_torques_internal instead
      /*
      my_connection.apply_torques(mock_time, mock_other_system,
      mock_first_index, mock_vector_system, mock_second_index);
      */
      my_connection.apply_torques(mock_time, mock_vector_system,
                                  mock_first_index, mock_other_system,
                                  mock_second_index);
      const int expected1 = (2 + 2);
      CHECK(my_connection.i_ == expected1);

      my_connection.apply_torques(mock_time, mock_list_system, mock_first_index,
                                  mock_other_system, mock_second_index);
      // doesnt compile, as expected, use apply_torques_internal instead
      /*
      my_connection.apply_torques(mock_time, mock_other_system,
      mock_first_index, mock_list_system, mock_second_index);
      */
      my_connection.apply_torques(mock_time, mock_list_system, mock_first_index,
                                  mock_other_system, mock_second_index);
      const int expected2 = expected1 - 2;
      CHECK(my_connection.i_ == expected2);

      // this is an ambiguous call without \elastica's overload resolution
      // mechanism
      // this should call this specialization of list<>, System
      my_connection.apply_torques(mock_time, mock_list_system, mock_first_index,
                                  mock_vector_system, mock_second_index);
      const int expected3 = 2 * expected2;
      CHECK(my_connection.i_ == expected3);

      // this should call this specialization of vector<>, list<>
      my_connection.apply_torques(mock_time, mock_vector_system,
                                  mock_first_index, mock_list_system,
                                  mock_second_index);
      CHECK(my_connection.i_ == 2 * expected3);
    };

    auto check_public_interface = [&]() {
      const int expected = my_connection.i_;
      my_connection.apply_torques_internal(mock_time, mock_vector_system,
                                           mock_first_index, mock_other_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_first_index, mock_vector_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == (expected + 2));

      my_connection.apply_torques_internal(mock_time, mock_list_system,
                                           mock_first_index, mock_other_system,
                                           mock_second_index);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_first_index, mock_list_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      // sadly this seems to call lhe list, System specialization, not the
      // list, vector specialization
      my_connection.apply_torques_internal(mock_time, mock_list_system,
                                           mock_first_index, mock_vector_system,
                                           mock_second_index);

      my_connection.apply_torques_internal(mock_time, mock_vector_system,
                                           mock_first_index, mock_list_system,
                                           mock_second_index);
      const int expected2 = 2 * 2 * expected;
      CHECK(my_connection.i_ == expected2);

      // no-operation
      my_connection.apply_forces_internal(mock_time, mock_vector_system,
                                          mock_first_index, mock_other_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_first_index, mock_vector_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected2);
      my_connection.apply_forces_internal(mock_time, mock_list_system,
                                          mock_first_index, mock_other_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_first_index, mock_list_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected2);
      my_connection.apply_forces_internal(mock_time, mock_vector_system,
                                          mock_first_index, mock_list_system,
                                          mock_second_index);
      my_connection.apply_forces_internal(mock_time, mock_list_system,
                                          mock_first_index, mock_vector_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected2);
    };

    // check constrain values for the above cases
    auto check_members_are_noop_for_nonoverloaded_types = [&]() {
      const int expected = my_connection.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      double mock_double_system(5.0);
      char mock_char_system('a');
      my_connection.apply_torques_internal(mock_time, mock_double_system,
                                           mock_first_index, mock_char_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_char_system,
                                           mock_first_index, mock_double_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      // check a template
      my_connection.apply_torques_internal(mock_time, mock_array_system,
                                           mock_first_index, mock_char_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_char_system,
                                           mock_first_index, mock_array_system,
                                           mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_double_system,
                                          mock_first_index, mock_char_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_char_system,
                                          mock_first_index, mock_double_system,
                                          mock_second_index);
      CHECK(my_connection.i_ == expected);
    };

    check_public_members();
    check_public_interface();
    check_members_are_noop_for_nonoverloaded_types();
  }
}  // namespace

// TODO : deferred connections

/// [dce]
namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(DeferredConnection) {
     public:
      explicit DeferredConnection(int& i) noexcept : i_(i){};
      DeferredConnection(DeferredConnection && other) noexcept : i_(other.i_) {}
      ~DeferredConnection() override = default;

      // overloaded for specialized systems

      // 1.a an integer specialization for forces
      template <typename TimeType, typename SecondSystem>
      void apply_forces(TimeType /*time*/, int& /*first_system*/,
                        SecondSystem& /*second_system*/) const {
        // do something
        ++i_;
      }

      // 1.b an integer specialization for torques
      template <typename TimeType, typename SecondSystem>
      void apply_torques(TimeType time, int& first_system,
                         SecondSystem& second_system) const {
        // do something
        apply_forces(time, first_system, second_system);
      }

      // Usually, you would need the following methods (2) too, with order of
      // arguments switched. Such a method should delegate to the above member
      // function since the algorithm should be independent of the ordering of
      // arguments, and depend only on their types
      // But because \elastica's detection mechanism is smart, you do not need
      // such a function and the above member function works for pairs of type
      // (int -- SecondSystem) or (SecondSystem -- int)!
      /*
      template <typename TimeType, typename FirstSystem>
      void apply_forces(TimeType time, FirstSystem & first_system, int&
      second_system) const {
        // do something, usually delegate to the other member
        apply_forces(time, second_system, second_index, first_system,
                     first_index);
      }

      template <typename TimeType, typename FirstSystem>
      void apply_torques(TimeType time, FirstSystem & first_system, int&
      second_system) const {
        // do something, usually delegate to the other member
        apply_torques(time, second_system, second_index, first_system,
                     first_index);
      }
       */

      // 3. Specialization when the first system is a float
      template <typename TimeType, typename SecondSystem>
      void apply_forces(TimeType /*time*/, float& /*first_system*/,
                        SecondSystem& /*second_system*/) const {
        // do something
        --i_;
      }

      template <typename TimeType, typename SecondSystem>
      void apply_torques(TimeType time, float& first_system,
                         SecondSystem& second_system) const {
        apply_forces(time, first_system, second_system);
      }

      // Similar to how (2) is unneeded for (1), you don't need the following
      // member function (4), the switched counterpart to (3)
      /*
      template <typename TimeType, typename FirstSystem>
      void apply_forces(TimeType time, FirstSystem & first_system,
                        float& second_system) const {
        // do something, usually delegate to the other member
        apply_forces(time, second_system, second_index, first_system,
                     first_index);
      }

      template <typename TimeType, typename FirstSystem>
      void apply_torques(TimeType time, FirstSystem & first_system,
                        float& second_system) const {
        // do something, usually delegate to the other member
        apply_torques(time, second_system, second_index, first_system,
                     first_index);
      }
       */

      // An added benefit to skipping the members (2) and (4) is when a pair of
      // systems  of type (int--float) gets called. If we had (2) and (4)
      // specified the function call to an (int--float) pair is likely to be
      // ambiguous since both (1) and (4) or (2) and (3) match. Now that we have
      // skipped (2) and (4), there is no conflict here

      // 5. maximal specialization when the first system is a float and the
      // second system is bool. Also works for first system bool and second
      // system float
      template <typename TimeType>
      void apply_forces(TimeType /*time*/, float& /*first_system*/,
                        bool& /*second_system*/) const {
        // do something
        i_ *= 2;
      }

      template <typename TimeType>
      void apply_torques(TimeType time, float& first_system,
                         bool& second_system) const {
        apply_forces(time, first_system, second_system);
      }

      // 6. a templated vector as first system
      template <typename TimeType, typename T, typename System>
      void apply_forces(
          TimeType /*time*/, std::vector<T>& /*first_system*/, System &
          /* second_system*/) const {
        // do something
        i_ += 2;
      }

      template <typename TimeType, typename T, typename System>
      void apply_torques(TimeType time, std::vector<T> & first_system,
                         System & second_system) const {
        apply_forces(time, first_system, second_system);
      }

      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept { return "DeferredConnection"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace connections

}  // namespace elastica
/// [dce]

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.Connections.Connection.deferred",
                     "[Unit][Simulator][Connections]") {
    int i = 2;
    using Conn = ::elastica::connections::DeferredConnection<>;
    auto my_connection(check_and_make_connection<Conn>(i));

    int mock_time = 3;
    bool mock_bool_system = false;
    int mock_int_system = 2;
    float mock_float_system = 2.0F;

    std::vector<int> mock_vector_system{1, 2, 3};
    std::list<int> mock_other_templated_system = {4, 5, 6};

    char mock_other_system = 'a';

    // check-metaprogramming
    {
      // int
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_int_system),
                                             decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_other_system),
                                             decltype(mock_int_system)>(),
          "connection metaprogramming failure!");

      // float
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_float_system),
                                             decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_other_system),
                                             decltype(mock_float_system)>(),
          "connection metaprogramming failure!");

      // float + int specialization
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_float_system),
                                             decltype(mock_int_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_torques<decltype(mock_time),
                                             decltype(mock_int_system),
                                             decltype(mock_float_system)>(),
          "connection metaprogramming failure!");

      // negation
      static_assert(Conn::derived_has_no_apply_torques<decltype(mock_time),
                                                       double, double>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_apply_torques<decltype(mock_time),
                                                       char, double>(),
                    "connection metaprogramming failure!");

      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_int_system),
                                            decltype(mock_float_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_apply_forces<decltype(mock_time),
                                            decltype(mock_float_system),
                                            decltype(mock_int_system)>(),
          "connection metaprogramming failure!");

      // deferred
      static_assert(Conn::derived_has_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_int_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_float_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(
          Conn::derived_has_deferred_apply_forces<
              decltype(mock_time), decltype(mock_float_system), bool>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_deferred_apply_torques<
              decltype(mock_time), decltype(mock_float_system), bool>(),
          "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_other_system),
                        decltype(mock_int_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_other_system),
                        decltype(mock_int_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_no_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_other_system),
                        decltype(mock_float_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_no_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_other_system),
                        decltype(mock_float_system)>(),
                    "connection metaprogramming failure!");

      static_assert(Conn::derived_has_deferred_apply_forces<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");
      static_assert(Conn::derived_has_deferred_apply_torques<
                        decltype(mock_time), decltype(mock_vector_system),
                        decltype(mock_other_system)>(),
                    "connection metaprogramming failure!");

      static_assert(
          Conn::derived_has_no_deferred_apply_forces<
              decltype(mock_time), decltype(mock_other_templated_system),
              decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
      static_assert(
          Conn::derived_has_no_deferred_apply_torques<
              decltype(mock_time), decltype(mock_other_templated_system),
              decltype(mock_other_system)>(),
          "connection metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_connection.apply_forces(mock_time, mock_int_system, mock_other_system);
      // doesnt compile, as expected, use apply_torques_internal instead
      /*
      my_connection.apply_torques(mock_time, mock_other_system,
      mock_int_system);
      */
      //
      my_connection.apply_torques(mock_time, mock_int_system,
                                  mock_other_system);
      const int expected1 = (2 + 2);
      CHECK(my_connection.i_ == expected1);

      my_connection.apply_forces(mock_time, mock_float_system,
                                 mock_other_system);
      // doesnt compile, as expected, use apply_torques_internal instead
      /*
      my_connection.apply_torques(mock_time, mock_other_system,
      mock_first_index, mock_float_system, mock_second_index);
      */
      //
      my_connection.apply_torques(mock_time, mock_float_system,
                                  mock_other_system);
      const int expected2 = expected1 - 2;
      CHECK(my_connection.i_ == expected2);

      // If done incorrectly, this is an ambiguous call with int, float
      // Since we provide an overload resolution mechanism in the base class,
      // we can have this around
      my_connection.apply_forces(mock_time, mock_float_system, mock_int_system);
      my_connection.apply_torques(mock_time, mock_float_system,
                                  mock_int_system);
      CHECK(my_connection.i_ == expected2 - 2);

      my_connection.apply_forces(mock_time, mock_int_system, mock_float_system);
      my_connection.apply_torques(mock_time, mock_int_system,
                                  mock_float_system);
      CHECK(my_connection.i_ == expected2);

      // specialization
      my_connection.apply_forces(mock_time, mock_float_system,
                                 mock_bool_system);
      my_connection.apply_torques(mock_time, mock_float_system,
                                  mock_bool_system);
      const int expected3 = expected2 * 4;
      CHECK(my_connection.i_ == expected3);

      // <vector>
      my_connection.apply_forces(mock_time, mock_vector_system,
                                 mock_int_system);
      my_connection.apply_torques(mock_time, mock_vector_system,
                                  mock_other_system);
      const int expected4 = expected3 + 4;
      CHECK(my_connection.i_ == expected4);
    };

    auto check_public_interface = [&]() {
      const int expected = my_connection.i_;
      // <int, X>
      my_connection.apply_forces_internal(mock_time, mock_int_system,
                                          mock_other_system);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_int_system);
      my_connection.apply_torques_internal(mock_time, mock_int_system,
                                           mock_other_system);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_int_system);
      CHECK(my_connection.i_ == (expected + 4));

      // <float, X>
      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_other_system);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_float_system);
      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_other_system);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_float_system);
      CHECK(my_connection.i_ == expected);

      // <float, bool> spec
      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_bool_system);
      my_connection.apply_forces_internal(mock_time, mock_bool_system,
                                          mock_float_system);
      const int expected2 = expected * 4;
      CHECK(my_connection.i_ == expected2);

      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_bool_system);
      my_connection.apply_torques_internal(mock_time, mock_bool_system,
                                           mock_float_system);
      const int expected3 = expected2 * 4;
      CHECK(my_connection.i_ == expected3);

      // <vector, X>
      my_connection.apply_forces_internal(mock_time, mock_vector_system,
                                          mock_other_system);
      my_connection.apply_forces_internal(mock_time, mock_other_system,
                                          mock_vector_system);
      my_connection.apply_torques_internal(mock_time, mock_vector_system,
                                           mock_other_system);
      my_connection.apply_torques_internal(mock_time, mock_other_system,
                                           mock_vector_system);
      const int expected4 = expected3 + 8;
      CHECK(my_connection.i_ == expected4);
    };

    // check constrain values for the above cases
    auto check_interface_is_noop_for_nonoverloaded_types = [&]() {
      const int expected = my_connection.i_;
      double mock_double_system(5.0);
      char mock_char_system('a');
      my_connection.apply_forces_internal(mock_time, mock_double_system,
                                          mock_char_system);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_double_system,
                                           mock_char_system);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_forces_internal(mock_time, mock_char_system,
                                          mock_double_system);
      CHECK(my_connection.i_ == expected);

      my_connection.apply_torques_internal(mock_time, mock_char_system,
                                           mock_double_system);
      CHECK(my_connection.i_ == expected);
    };

    auto check_interface_for_fully_specialized_types = [&]() {
      const int expected = my_connection.i_;
      float mock_float_system(5.0F);
      bool mock_bool_system(true);

      my_connection.apply_forces_internal(mock_time, mock_float_system,
                                          mock_bool_system);
      const int expected2 = 2 * expected;
      CHECK(my_connection.i_ == expected2);

      const int expected3 = 2 * expected2;
      my_connection.apply_torques_internal(mock_time, mock_float_system,
                                           mock_bool_system);
      CHECK(my_connection.i_ == expected3);

      const int expected4 = 2 * expected3;
      my_connection.apply_torques_internal(mock_time, mock_bool_system,
                                           mock_float_system);
      CHECK(my_connection.i_ == expected4);

      const int expected5 = 2 * expected4;
      my_connection.apply_forces_internal(mock_time, mock_bool_system,
                                          mock_float_system);
      CHECK(my_connection.i_ == expected5);
    };

    check_public_members();
    check_public_interface();
    check_interface_is_noop_for_nonoverloaded_types();
    check_interface_for_fully_specialized_types();
  }
}  // namespace
