//
// Created by Tejaswin Parthasarathy on 1/22/21.
//

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Connections/MakeConnection.hpp"
#include "Simulator/Connections/Protocols.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {
  using namespace elastica::connections;
  using namespace elastica;

  struct A {
    template <typename TimeType, typename FirstSystemType,
              typename FirstIndexType, typename SecondSystemType,
              typename SecondIndexType>
    void operator()(TimeType, FirstSystemType&, FirstIndexType,
                    SecondSystemType&, SecondIndexType) const {
      ++times_called;
    }
    mutable int times_called = {0};
  };

  struct B {
    explicit B(int& g) : times_called(g){};
    template <typename TimeType, typename FirstSystemType,
              typename FirstIndexType, typename SecondSystemType,
              typename SecondIndexType>
    void operator()(TimeType, FirstSystemType&, FirstIndexType,
                    SecondSystemType&, SecondIndexType) const {
      ++times_called;
    }
    int& times_called;
  };

  void test_make_connection_for_lvalue_references() {
    auto a{A()};  // 1 created
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1

    // a and b are captured by references inside, and copies are made
    // in connection_factory upon invocation
    auto connection_factory = make_connection_detail::make_connection(a, b);
    // do static assert once for all make_forcing
    static_assert(::tt::assert_conforms_to<decltype(connection_factory()),
                                           protocols::Connection>,
                  "Protocol violation!");

    // now make a connection
    auto my_connection = connection_factory();

    // check priori
    CHECK(a.times_called == 0);
    CHECK(b.times_called == 0);

    // connection interface
    int mock_time = 2;
    int mock_first_system = 3;
    std::size_t mock_first_index = 1;
    int mock_second_system = 3;
    std::size_t mock_second_index = 1;

    // calls a 1 times
    my_connection.apply_forces_internal(mock_time, mock_first_system,
                                        mock_first_index, mock_second_system,
                                        mock_second_index);
    // calls B 2 times
    my_connection.apply_torques_internal(mock_time, mock_first_system,
                                         mock_first_index, mock_second_system,
                                         mock_second_index);

    // check the number of times the functor is called
    // will be 0 because it is copied as expected
    CHECK(a.times_called == 0);
    // but b had a reference and it is updated there
    CHECK(b.times_called == 1);

    my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                               mock_second_system, mock_second_index);
    my_connection.apply_torques(mock_time, mock_first_system, mock_first_index,
                                mock_second_system, mock_second_index);

    CHECK(b.times_called == 2);

    auto new_connection = connection_factory();
    CHECK(std::addressof(new_connection) != std::addressof(my_connection));
  }

  struct AggregateB {
    explicit AggregateB(std::string s) : s_(std::move(s)){};
    template <typename TimeType, typename FirstSystemType,
              typename FirstIndexType, typename SecondSystemType,
              typename SecondIndexType>
    void operator()(TimeType, FirstSystemType& s, FirstIndexType,
                    SecondSystemType&, SecondIndexType) const {
      ++s;
    }
    std::string s_;
  };

  void test_make_connection_for_xvalues() {
    auto b1{AggregateB("Melt")};
    auto b2{AggregateB("Gibsont")};

    auto connection_factory =
        make_connection_detail::make_connection(std::move(b1), std::move(b2));

    // now make a connection
    auto my_connection = connection_factory();

    int mock_time = 3;
    int mock_first_system = 2;
    std::size_t mock_first_index = 1;
    int mock_second_system = 3;
    std::size_t mock_second_index = 1;

    my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                               mock_second_system, mock_second_index);
    CHECK(mock_first_system == 3);
    my_connection.apply_torques(mock_time, mock_first_system, mock_first_index,
                                mock_second_system, mock_second_index);
    CHECK(mock_first_system == 4);

    auto new_connection = connection_factory();
    CHECK(std::addressof(new_connection) != std::addressof(my_connection));
  }

  void test_make_connection_for_prvalues() {
    // a and b are captured by references inside, and copies are made
    // in connection_factory upon invocation
    int value_called = 0;
    auto connection_factory = make_connection_detail::make_connection(
        [&value_called](auto...) { ++value_called; }, [](auto...) {});

    // now make a connection
    auto my_connection = connection_factory();
    // connection interface
    int mock_time = 2;
    int mock_first_system = 2;
    std::size_t mock_first_index = 1;
    int mock_second_system = 3;
    std::size_t mock_second_index = 1;

    my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                               mock_second_system, mock_second_index);
    CHECK(value_called == 1);
    value_called = 0;
    my_connection.apply_forces_internal(mock_time, mock_first_system,
                                        mock_first_index, mock_second_system,
                                        mock_second_index);
    CHECK(value_called == 1);

    auto new_connection = connection_factory();
    new_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                                mock_second_system, mock_second_index);
    CHECK(value_called == 2);
    CHECK(std::addressof(new_connection) != std::addressof(my_connection));
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Connections.MakeConnection.implementation",
                     "[Unit][Simulator][Connections]") {
    // These always create a copy of the functors in question
    test_make_connection_for_lvalue_references();
    test_make_connection_for_prvalues();
    test_make_connection_for_xvalues();
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Connections.MakeConnection.switched_arguments",
      "[Unit][Simulator][Connections]") {
    {  // forces first
      int value_called = 0;
      auto connection_factory = make_connection_detail::make_connection(
          [&value_called](auto /* time */, int& /*first_system*/,
                          int /*first_index*/, double& /*second_system*/,
                          int /* second_index */) { ++value_called; },
          [](auto...) {});

      // now make a connection
      auto my_connection = connection_factory();

      // connection interface
      int mock_time = 2;
      double mock_first_system = 5.0;
      std::size_t mock_first_index = 1;
      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      // deliberately switched arguments here
      my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                                 mock_second_system, mock_second_index);
      CHECK(value_called == 1);

      my_connection.apply_forces(mock_time, mock_second_system,
                                 mock_first_index, mock_first_system,
                                 mock_second_index);
      CHECK(value_called == 2);

      // see if internal apply has the same property
      my_connection.apply_forces_internal(mock_time, mock_first_system,
                                          mock_first_index, mock_second_system,
                                          mock_second_index);
      CHECK(value_called == 3);

      my_connection.apply_forces_internal(mock_time, mock_second_system,
                                          mock_first_index, mock_first_system,
                                          mock_second_index);
      CHECK(value_called == 4);
    }

    {  // forces first
      int value_called = 0;
      auto connection_factory = make_connection_detail::make_connection(
          [](auto...) {},
          [&value_called](auto /* time */, int& /*first_system*/,
                          int /*first_index*/, double& /*second_system*/,
                          int /* second_index */) { ++value_called; });

      // now make a connection
      auto my_connection = connection_factory();

      // connection interface
      int mock_time = 2;
      double mock_first_system = 5.0;
      std::size_t mock_first_index = 1;
      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      // deliberately switched arguments here
      my_connection.apply_torques(mock_time, mock_first_system,
                                  mock_first_index, mock_second_system,
                                  mock_second_index);
      CHECK(value_called == 1);

      my_connection.apply_torques(mock_time, mock_second_system,
                                  mock_first_index, mock_first_system,
                                  mock_second_index);
      CHECK(value_called == 2);

      // see if internal apply has the same property
      my_connection.apply_torques_internal(mock_time, mock_first_system,
                                           mock_first_index, mock_second_system,
                                           mock_second_index);
      CHECK(value_called == 3);

      my_connection.apply_torques_internal(mock_time, mock_second_system,
                                           mock_first_index, mock_first_system,
                                           mock_second_index);
      CHECK(value_called == 4);
    }
  }

  struct ANC : elastica::NonCopyable {
    template <typename TimeType, typename FirstSystemType,
              typename FirstIndexType, typename SecondSystemType,
              typename SecondIndexType, typename... Args>
    void operator()(TimeType, FirstSystemType& s, FirstIndexType,
                    SecondSystemType, SecondIndexType, Args...) const {
      ++s;
    }
    mutable int times_called = {0};
  };

  void test_make_connection_for_noncopyable_values() {
    auto a{ANC()};
    auto connection_factory = make_connection_detail::make_connection(
        std::move(a), [](auto, auto, auto, auto...) {});
    // now make a connection
    auto my_connection = connection_factory();
    // connection interface
    int mock_time = 3;
    int mock_first_system = 2;
    std::size_t mock_first_index = 1;
    int mock_second_system = 3;
    std::size_t mock_second_index = 1;

    my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                               mock_second_system, mock_second_index);
    CHECK(mock_first_system == 3);
  }

  void test_make_connection_for_noncopyable_rates() {
    auto a{ANC()};
    auto connection_factory = make_connection_detail::make_connection(
        [](auto, auto, auto, auto...) {}, std::move(a));
    // now make a connection
    auto my_connection = connection_factory();
    // connection interface
    int mock_time = 3;
    int mock_first_system = 2;
    std::size_t mock_first_index = 1;
    int mock_second_system = 3;
    std::size_t mock_second_index = 1;

    my_connection.apply_torques(mock_time, mock_first_system, mock_first_index,
                                mock_second_system, mock_second_index);
    CHECK(mock_first_system == 3);
  }

  void test_make_connection_for_both_noncopyables() {
    auto a1{ANC()};
    auto a2{ANC()};
    auto connection_factory =
        make_connection_detail::make_connection(std::move(a1), std::move(a2));
    // now make a connection
    auto my_connection = connection_factory();
    // connection interface
    int mock_time = 3;
    int mock_first_system = 2;
    std::size_t mock_first_index = 1;
    int mock_second_system = 3;
    std::size_t mock_second_index = 1;

    my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                               mock_second_system, mock_second_index);
    CHECK(mock_first_system == 3);
    my_connection.apply_torques(mock_time, mock_first_system, mock_first_index,
                                mock_second_system, mock_second_index);
    CHECK(mock_first_system == 4);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Connections.MakeConnection.noncopyables",
                     "[Unit][Simulator][Connections]") {
    // These always create a copy of the functors in question
    test_make_connection_for_noncopyable_values();
    test_make_connection_for_noncopyable_rates();
    test_make_connection_for_both_noncopyables();
  }

  void test_make_connection_throws_for_noncopyable_values() {
    auto a{ANC()};
    auto connection_factory = make_connection_detail::make_connection(
        std::move(a), [](auto, auto, auto, auto...) {});
    auto fc = connection_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(connection_factory(),
                      Catch::Contains("the first functor is non-copyable"));
  }

  void test_make_connection_throws_for_noncopyable_rates() {
    auto a{ANC()};
    auto connection_factory = make_connection_detail::make_connection(
        [](auto, auto, auto, auto...) {}, std::move(a));
    auto fc = connection_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(connection_factory(),
                      Catch::Contains("the second functor is non-copyable"));
  }

  void test_make_connection_throws_for_both_noncopyables() {
    auto a1{ANC()};
    auto a2{ANC()};
    auto connection_factory =
        make_connection_detail::make_connection(std::move(a1), std::move(a2));
    auto fc = connection_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(connection_factory(),
                      Catch::Contains("both the functors are non-copyable"));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Connections.MakeConnection.throws_for_repeated_use_of_noncopyables",
      "[Unit][Simulator][Connections]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_connection_throws_for_noncopyable_values();
    test_make_connection_throws_for_noncopyable_rates();
    test_make_connection_throws_for_both_noncopyables();
  }

  struct CountedA : counter<CountedA> {
    template <typename TimeType, typename FirstSystemType,
              typename FirstIndexType, typename SecondSystemType,
              typename SecondIndexType>
    void operator()(TimeType, FirstSystemType&, FirstIndexType,
                    SecondSystemType&, SecondIndexType) const {}
  };

  struct CountedB : counter<CountedB> {
    explicit CountedB(int g) : times_called(g){};
    template <typename TimeType, typename FirstSystemType,
              typename FirstIndexType, typename SecondSystemType,
              typename SecondIndexType>
    void operator()(TimeType, FirstSystemType&, FirstIndexType,
                    SecondSystemType&, SecondIndexType) const {
      ++times_called;
    }
    mutable int times_called;
  };

  IGNORE_UNUSED void
  test_make_connection_statistics_for_both_lvalue_references() {
    auto a{CountedA()};   // 1 created
    auto b{CountedB(2)};  // 1

    CHECK(counter<CountedA>::objects_alive == 1);  // 1 object is a
    CHECK(counter<CountedA>::objects_copied == 0);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 1);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 0);
    CHECK(counter<CountedB>::objects_moved == 0);

    // a and b are captured by references inside, and copies are made
    // in connection_factory upon invocation
    auto connection_factory = make_connection_detail::make_connection(a, b);

    CHECK(counter<CountedA>::objects_alive == 1);  // 1 object is a
    CHECK(counter<CountedA>::objects_copied == 0);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 1);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 0);
    CHECK(counter<CountedB>::objects_moved == 0);

    // now make a connection
    auto my_connection = connection_factory();

    CHECK(counter<CountedA>::objects_alive == 2);
    CHECK(counter<CountedA>::objects_copied == 1);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 2);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 1);
    CHECK(counter<CountedB>::objects_moved == 0);

    elastica::ignore_unused(my_connection);
  }

  void test_make_connection_user_interface() {
    auto a{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // xvalues and prvalues

      /// [make_torque_example]
      auto connection_factory = make_connection(
          make_force(std::move(a)),
          make_torque([](auto /*time*/, auto& /*first_sys*/,
                         auto /* first_index*/, auto& /*second_sys*/,
                         auto /* second_index*/) {}));
      /// [make_torque_example]

      // now make a connection
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_first_system = 2;
      std::size_t mock_first_index = 1;
      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_first_system == 3);
    }

    {  // xvalues and prvalues

      /// [make_force_example]
      auto connection_factory = make_connection(
          make_torque(std::move(c)),
          make_force([](auto /*time*/, auto& /*first_sys*/,
                        auto /* first_index*/, auto& /*second_sys*/,
                        auto /* second_index*/) {}));
      /// [make_force_example]

      // now make a connection
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_first_system = 2;
      std::size_t mock_first_index = 1;
      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_torques(mock_time, mock_first_system,
                                  mock_first_index, mock_second_system,
                                  mock_second_index);
      CHECK(mock_first_system == 3);
    }

    {  // lvalues
      auto connection_factory = make_connection(make_force(b), make_torque(b));
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_first_system = 2;
      std::size_t mock_first_index = 1;
      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_torques(mock_time, mock_first_system,
                                  mock_first_index, mock_second_system,
                                  mock_second_index);
      // referece update
      CHECK(b.times_called == 1);
      my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                                 mock_second_system, mock_second_index);
      // referece update
      CHECK(b.times_called == 2);
    }

    {
      /// [make_connection_example]
      // This makes a connection with two functors
      // however passing the functors directly is not advised and so forbidden
      // rather pass them through make_force() and
      // make_torque(), to clearly highlight which function is which
      // for the reader
      auto connection_factory = make_connection(
          make_force([](auto /*time*/, auto& /*first_sys*/,
                        auto /* first_index*/, auto& /*second_sys*/,
                        auto /* second_index*/
                     ) {}),
          make_torque([](auto /*time*/, auto& /*first_sys*/,
                         auto /*first_index*/, auto& /*second_sys*/,
                         auto /* second_index*/
                      ) {}));

      // this only makes a class
      // we now need to instantiate that class to use it
      // and we do that by calling the factory's operator() method

      auto my_connection = connection_factory();

      // now we can use my_connection(), which has the interface of a
      // elastica connection object:

      // my_connection.apply_forces(...);
      // my_connection.apply_torques(...);

      // Additionally, order of make_force and make_torque don't matter
      auto another_forcing_factory = make_connection(
          make_torque([](auto /*time*/, auto& /*first_sys*/,
                         auto /* first_index*/, auto& /*second_sys*/,
                         auto /* second_index*/) {}),
          make_force([](auto /*time*/, auto& /*first_sys*/,
                        auto /* first_index*/, auto& /*second_sys*/,
                        auto /* second_index*/) {}));

      /// [make_connection_example]
      elastica::ignore_unused(my_connection);
      elastica::ignore_unused(another_forcing_factory);
    }
  }

  void test_make_connection_user_interface_parametrized() {
    // two ways of doing it

    // cleaner, but requires programmer to know basic C++
    /// [make_parametrized_connection_example]
    auto connection_factory = [](int x, int y) {
      return make_connection(
          make_force([=](auto /*time*/, auto& first_sys, auto /* first_index*/,
                         auto& /*second_sys*/,
                         auto /* second_index*/) { first_sys += x; }),
          make_torque([=](auto /*time*/, auto& first_sys, auto /* first_index*/,
                          auto& /*second_sys*/,
                          auto /* second_index*/) { first_sys += y; }))();
    };
    // The above connection is parameterized on x and y
    // and this is its usage

    // auto my_connection = connection_factory(2, 1);

    // now you can use my_connection similar to the other example!
    /// [make_parametrized_connection_example]

    /*
    // much more confusing, requires programmer to return a lambda within a
    // lambda, also repeating syntax
    auto new_connection_factory = [](int x, int y) {
      return make_connection(
          make_torque([](int x, int y) {
            return [x](auto& sys, auto, auto, auto...) { sys += x; };
          }),
          make_force([](int x, int y) {
            return [y](auto& sys, auto, auto, auto...) { sys += y; };
          }));
    };
    */

    {  // lvalues
      auto my_connection = connection_factory(2, 1);
      // connection interface
      int mock_time = 3;
      int mock_first_system = 2;
      std::size_t mock_first_index = 1;
      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_forces(mock_time, mock_first_system, mock_first_index,
                                 mock_second_system, mock_second_index);
      // referece update
      CHECK(mock_first_system == 4);
      // referece update
      my_connection.apply_torques(mock_time, mock_first_system,
                                  mock_first_index, mock_second_system,
                                  mock_second_index);
      CHECK(mock_first_system == 5);

      auto new_connection = connection_factory(2, 1);
      new_connection.apply_forces(mock_time, mock_first_system,
                                  mock_first_index, mock_second_system,
                                  mock_second_index);
      // referece update
      CHECK(mock_first_system == 7);
      // referece update
      new_connection.apply_torques(mock_time, mock_first_system,
                                   mock_first_index, mock_second_system,
                                   mock_second_index);
      CHECK(mock_first_system == 8);

      auto another_connection = connection_factory(10, 20);
      another_connection.apply_forces(mock_time, mock_first_system,
                                      mock_first_index, mock_second_system,
                                      mock_second_index);
      // referece update
      CHECK(mock_first_system == 18);
      // referece update
      another_connection.apply_torques(mock_time, mock_first_system,
                                       mock_first_index, mock_second_system,
                                       mock_second_index);
      CHECK(mock_first_system == 38);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Connections.MakeConnection.user_interface",
      "[Unit][Simulator][Connections]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_connection_user_interface();
    test_make_connection_user_interface_parametrized();
  }

  void test_make_connection_overloaded_forces_interface() {
    auto temp1{ANC()};
    auto temp2{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // prforces and xforces

      // First test with only two, then do more
      auto connection_factory = make_connection(
          make_force([](auto /*time*/, int& system, auto /*first_index*/,
                        auto& /*second_system*/,
                        auto /*second_index*/) { system += 1; },
                     [](auto /*time*/, float& system, auto /*first_index*/,
                        auto& /*second_system*/,
                        auto /*second_index*/) { system -= 0.5F; }),
          make_torque(std::move(temp1)));

      // now make a connection
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_int_system = 2;
      int mock_index = 1;

      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_forces(mock_time, mock_int_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_connection.apply_forces(mock_time, mock_float_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(0.3F));
    }

    {
      // First test with only two, then do more
      /// [make_force_overloaded_example]
      // assuming the second system is always an integer here, WLOG
      auto connection_factory = make_connection(
          make_force([](auto /*time*/, int& system, auto /*first_index*/,
                        int& /*second_system*/,
                        auto /*second_index*/) { system += 1; },
                     [](auto /*time*/, float& system, auto /*first_index*/,
                        int& /*second_system*/,
                        auto /*second_index*/) { system -= 0.5F; },
                     [](auto /*time*/, double& system, auto /*first_index*/,
                        int& /*second_system*/,
                        auto /*second_index*/) { system -= 0.8; },
                     [](auto /*time*/, char& system, auto /*first_index*/,
                        int& /*second_system*/,
                        auto /*second_index*/) { system = 'b'; },

                     [](auto /*time*/, auto& /*system*/, auto /*first_index*/,
                        int& /*second_system*/, auto /*second_index*/) {
                       // no-operation as a default fallback for all other
                       // systems, can be skipped so as to generate compile-time
                       // errors in case your custom connection is not meant to
                       // be mapped to arbitrary systems!
                     }),
          make_torque(std::move(temp2)));
      /// [make_force_overloaded_example]

      // now make a connection
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_int_system = 2;
      int mock_index = 1;

      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_forces(mock_time, mock_int_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_connection.apply_forces(mock_time, mock_float_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(0.3F));

      double mock_double_system = 0.2;
      my_connection.apply_forces(mock_time, mock_double_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_double_system == approx(-0.6));

      char mock_char_system = 'p';
      my_connection.apply_forces(mock_time, mock_char_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_char_system == 'b');

      // no-operation for bool type say
      bool mock_bool_system = true;
      my_connection.apply_forces(mock_time, mock_bool_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_bool_system);
    }

    {  // xforces and prforces

      auto fover = [](auto /*time*/, float& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system -= 0.5F; };

      auto connection_factory = make_connection(
          make_torque(std::move(c)),
          make_force([](auto /*time*/, int& system, auto /*first_index*/,
                        auto& /*second_system*/,
                        auto /*second_index*/) { system += 1; },
                     std::move(fover)));

      // now make a connection
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_system = 2;
      int mock_index = 1;

      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_forces(mock_time, mock_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_system == 3);

      float mock_float_system(1.5F);
      my_connection.apply_forces(mock_time, mock_float_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system += 1.5; };

      auto cover = [](auto /*time*/, char& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system = 'b'; };

      auto new_connection_factory = make_connection(
          make_torque([](auto /*time*/, int& system, auto /*first_index*/,
                         auto& /*second_system*/,
                         auto /*second_index*/) { system += 1; }),
          make_force([](auto /*time*/, int& system, auto /*first_index*/,
                        auto& /*second_system*/,
                        auto /*second_index*/) { system += 1; },
                     std::move(dover), std::move(cover),
                     [](auto /*time*/, float& system, auto /*first_index*/,
                        auto& /*second_system*/,
                        auto /*second_index*/) { system -= 0.5F; }));

      // now make a new_connection
      auto my_new_connection = new_connection_factory();
      // new_connection interface
      CHECK(mock_system == 3);
      my_new_connection.apply_forces(mock_time, mock_system, mock_index,
                                     mock_second_system, mock_second_index);
      CHECK(mock_system == 4);

      my_new_connection.apply_forces(mock_time, mock_float_system, mock_index,
                                     mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(0.5F));
    }

    {  // lforces

      const auto fover = [](auto /*time*/, float& system, auto /*first_index*/,
                            auto& /*second_system*/,
                            auto /*second_index*/) { system -= 0.5F; };
      auto iover = [](auto /*time*/, int& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system += 1; };

      auto connection_factory =
          make_connection(make_force(fover, iover), make_torque(b));

      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_int_system = 2;
      int mock_index = 1;

      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_torques(mock_time, mock_int_system, mock_index,
                                  mock_second_system, mock_second_index);
      // referece update
      CHECK(b.times_called == 1);

      CHECK(mock_int_system == 2);
      my_connection.apply_forces(mock_time, mock_int_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_int_system == 3);

      float mock_float_system(1.5F);
      my_connection.apply_forces(mock_time, mock_float_system, mock_index,
                                 mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system += 1.5; };

      auto cover = [](auto /*time*/, char& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system = 'b'; };

      auto new_connection_factory = make_connection(
          make_force(fover, iover, dover, cover), make_torque(b));

      auto my_new_connection = new_connection_factory();
      // connection interface
      my_new_connection.apply_torques(mock_time, mock_int_system, mock_index,
                                      mock_second_system, mock_second_index);
      // referece update
      CHECK(b.times_called == 2);

      CHECK(mock_int_system == 3);
      my_new_connection.apply_forces(mock_time, mock_int_system, mock_index,
                                     mock_second_system, mock_second_index);
      CHECK(mock_int_system == 4);

      my_new_connection.apply_forces(mock_time, mock_float_system, mock_index,
                                     mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(0.5F));

      double mock_double_system = 0.1;
      my_new_connection.apply_forces(mock_time, mock_double_system, mock_index,
                                     mock_second_system, mock_second_index);
      CHECK(mock_double_system == approx(1.6));

      char mock_char_system = 'p';
      my_new_connection.apply_forces(mock_time, mock_char_system, mock_index,
                                     mock_second_system, mock_second_index);
      CHECK(mock_char_system == 'b');
    }
  }

  void test_make_connection_overloaded_torques_interface() {
    auto temp1{ANC()};
    auto temp2{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // prtorques and torquesx

      // First test with only two, then do more
      auto connection_factory = make_connection(
          make_torque([](auto /*time*/, int& system, auto /*first_index*/,
                         auto& /*second_system*/,
                         auto /*second_index*/) { system += 1; },
                      [](auto /*time*/, float& system, auto /*first_index*/,
                         auto& /*second_system*/,
                         auto /*second_index*/) { system -= 0.5F; }),
          make_force(std::move(temp1)));

      // now make a connection
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_int_system = 2;
      int mock_index = 1;

      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_torques(mock_time, mock_int_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_connection.apply_torques(mock_time, mock_float_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(0.3F));
    }

    {
      // First test with only two, then do more
      /// [make_torque_overloaded_example]
      // assuming the second system is always an integer here, WLOG
      auto connection_factory = make_connection(
          make_torque([](auto /*time*/, int& system, auto /*first_index*/,
                         int& /*second_system*/,
                         auto /*second_index*/) { system += 1; },
                      [](auto /*time*/, float& system, auto /*first_index*/,
                         int& /*second_system*/,
                         auto /*second_index*/) { system -= 0.5F; },
                      [](auto /*time*/, double& system, auto /*first_index*/,
                         int& /*second_system*/,
                         auto /*second_index*/) { system -= 0.8; },
                      [](auto /*time*/, char& system, auto /*first_index*/,
                         int& /*second_system*/,
                         auto /*second_index*/) { system = 'b'; },
                      [](auto /*time*/, auto& /*system*/, auto /*first_index*/,
                         auto& /*second_system*/, auto /*second_index*/) {
                        // no-operation as a default fallback for all other
                        // systems, can be skipped so as to generate
                        // compile-time errors in case your custom connection is
                        // not meant to be mapped to arbitrary systems!
                      }),
          make_force(std::move(temp2)));
      /// [make_torque_overloaded_example]

      // now make a connection
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_int_system = 2;
      int mock_index = 1;

      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_torques(mock_time, mock_int_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_connection.apply_torques(mock_time, mock_float_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(0.3F));

      double mock_double_system = 0.2;
      my_connection.apply_torques(mock_time, mock_double_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_double_system == approx(-0.6));

      char mock_char_system = 'p';
      my_connection.apply_torques(mock_time, mock_char_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_char_system == 'b');

      // no-operation for bool type say
      bool mock_bool_system = true;
      my_connection.apply_torques(mock_time, mock_bool_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_bool_system);
    }

    {  // torquesx and prtorques

      auto fover = [](auto /*time*/, float& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system -= 0.5F; };

      auto connection_factory = make_connection(
          make_force(std::move(c)),
          make_torque([](auto /*time*/, int& system, auto /*first_index*/,
                         auto& /*second_system*/,
                         auto /*second_index*/) { system += 1; },
                      std::move(fover)));

      // now make a connection
      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_system = 2;
      int mock_index = 1;

      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_torques(mock_time, mock_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_system == 3);

      float mock_float_system(1.5F);
      my_connection.apply_torques(mock_time, mock_float_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system += 1.5; };

      auto cover = [](auto /*time*/, char& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system = 'b'; };

      auto new_connection_factory = make_connection(
          make_force([](auto /*time*/, int& system, auto /*first_index*/,
                        auto& /*second_system*/,
                        auto /*second_index*/) { system += 1; }),
          make_torque([](auto /*time*/, int& system, auto /*first_index*/,
                         auto& /*second_system*/,
                         auto /*second_index*/) { system += 1; },
                      std::move(dover), std::move(cover),
                      [](auto /*time*/, float& system, auto /*first_index*/,
                         auto& /*second_system*/,
                         auto /*second_index*/) { system -= 0.5F; }));

      // now make a new_connection
      auto my_new_connection = new_connection_factory();
      // new_connection interface
      CHECK(mock_system == 3);
      my_new_connection.apply_torques(mock_time, mock_system, mock_index,
                                      mock_second_system, mock_second_index);
      CHECK(mock_system == 4);

      my_new_connection.apply_torques(mock_time, mock_float_system, mock_index,
                                      mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(0.5F));
    }

    {  // ltorques

      const auto fover = [](auto /*time*/, float& system, auto /*first_index*/,
                            auto& /*second_system*/,
                            auto /*second_index*/) { system -= 0.5F; };
      auto iover = [](auto /*time*/, int& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system += 1; };

      auto connection_factory =
          make_connection(make_torque(fover, iover), make_force(b));

      auto my_connection = connection_factory();
      // connection interface
      int mock_time = 3;
      int mock_int_system = 2;
      int mock_index = 1;

      int mock_second_system = 3;
      std::size_t mock_second_index = 1;

      my_connection.apply_forces(mock_time, mock_int_system, mock_index,
                                 mock_second_system, mock_second_index);
      // referece update
      CHECK(b.times_called == 1);

      CHECK(mock_int_system == 2);
      my_connection.apply_torques(mock_time, mock_int_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_int_system == 3);

      float mock_float_system(1.5F);
      my_connection.apply_torques(mock_time, mock_float_system, mock_index,
                                  mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*first_index*/,
                      auto& /*second_system*/,
                      auto /*second_index*/) { system += 1.5; };

      const auto cover = [](auto /*time*/, char& system, auto /*first_index*/,
                            auto& /*second_system*/,
                            auto /*second_index*/) { system = 'b'; };

      auto new_connection_factory = make_connection(
          make_torque(fover, iover, dover, cover), make_force(b));

      auto my_new_connection = new_connection_factory();
      // connection interface
      my_new_connection.apply_forces(mock_time, mock_int_system, mock_index,
                                     mock_second_system, mock_second_index);
      // referece update
      CHECK(b.times_called == 2);

      CHECK(mock_int_system == 3);
      my_new_connection.apply_torques(mock_time, mock_int_system, mock_index,
                                      mock_second_system, mock_second_index);
      CHECK(mock_int_system == 4);

      my_new_connection.apply_torques(mock_time, mock_float_system, mock_index,
                                      mock_second_system, mock_second_index);
      CHECK(mock_float_system == approx(0.5F));

      double mock_double_system = 0.1;
      my_new_connection.apply_torques(mock_time, mock_double_system, mock_index,
                                      mock_second_system, mock_second_index);
      CHECK(mock_double_system == approx(1.6));

      char mock_char_system = 'p';
      my_new_connection.apply_torques(mock_time, mock_char_system, mock_index,
                                      mock_second_system, mock_second_index);
      CHECK(mock_char_system == 'b');
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Connections.MakeConnection.overloaded_interface",
      "[Unit][Simulator][Connections]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_connection_overloaded_forces_interface();
    test_make_connection_overloaded_torques_interface();
  }
}  // namespace
