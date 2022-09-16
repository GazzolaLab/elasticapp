//
// Created by Tejaswin Parthasarathy on 1/8/21.
//

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Forcing/MakeForcing.hpp"
#include "Simulator/Forcing/Protocols.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {
  using namespace elastica::forcing;
  using namespace elastica;

  struct A {
    template <typename TimeType, typename SystemType, typename IndexType>
    void operator()(TimeType, SystemType&, IndexType) const {
      ++times_called;
    }
    mutable int times_called = {0};
  };

  struct B {
    explicit B(int& g) : times_called(g){};
    template <typename TimeType, typename SystemType, typename IndexType>
    void operator()(TimeType, SystemType&, IndexType) const {
      ++times_called;
    }
    int& times_called;
  };

  void test_make_forcing_for_lvalue_references() {
    auto a{A()};  // 1 created
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1

    // a and b are captured by references inside, and copies are made
    // in forcing_factory upon invocation
    auto forcing_factory = make_forcing_detail::make_forcing(a, b);
    // do static assert once for all make_forcing
    static_assert(::tt::assert_conforms_to<decltype(forcing_factory()),
                                           protocols::Forcing>,
                  "Protocol violation!");

    // now make a constrint
    auto my_forcing = forcing_factory();

    // check priori
    CHECK(a.times_called == 0);
    CHECK(b.times_called == 0);

    // forcing interface
    int mock_time = 3;
    int mock_system = 2;

    // calls a 6 times
    my_forcing.apply_forces_internal(mock_time, mock_system,
                                     numeric_at(1UL, 2UL, 3UL, 4UL, 5UL, 6UL));
    // calls B 3 times
    my_forcing.apply_torques_internal(mock_time, mock_system,
                                      numeric_at(1UL, 2UL, 3UL));

    // check the number of times the functor is called
    // will be 0 because it is copied as expected
    CHECK(a.times_called == 0);
    // but b had a reference and it is updated there
    CHECK(b.times_called == 3);

    my_forcing.apply_forces(mock_time, mock_system, 1);
    my_forcing.apply_torques(mock_time, mock_system, 3);

    CHECK(b.times_called == 4);

    auto new_forcing = forcing_factory();
    CHECK(std::addressof(new_forcing) != std::addressof(my_forcing));
  }

  struct AggregateB {
    explicit AggregateB(std::string s) : s_(std::move(s)){};
    template <typename TimeType, typename SystemType, typename IndexType>
    void operator()(TimeType, SystemType& s, IndexType) const {
      ++s;
    }
    std::string s_;
  };

  void test_make_forcing_for_xvalues() {
    auto b1{AggregateB("Melt")};
    auto b2{AggregateB("Gibsont")};

    auto forcing_factory =
        make_forcing_detail::make_forcing(std::move(b1), std::move(b2));

    // now make a constrint
    auto my_forcing = forcing_factory();

    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;

    my_forcing.apply_forces(mock_time, mock_system, mock_index);
    CHECK(mock_system == 3);
    my_forcing.apply_torques(mock_time, mock_system, mock_index);
    CHECK(mock_system == 4);

    auto new_forcing = forcing_factory();
    CHECK(std::addressof(new_forcing) != std::addressof(my_forcing));
  }

  void test_make_forcing_for_prvalues() {
    // a and b are captured by references inside, and copies are made
    // in forcing_factory upon invocation
    int value_called = 0;
    auto forcing_factory = make_forcing_detail::make_forcing(
        [&value_called](auto, auto, auto) { ++value_called; },
        [](auto, auto, auto) {});

    // now make a constrint
    auto my_forcing = forcing_factory();
    // forcing interface
    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;
    my_forcing.apply_forces(mock_time, mock_system, mock_index);
    CHECK(value_called == 1);
    value_called = 0;
    my_forcing.apply_forces_internal(mock_time, mock_system,
                                     numeric_at(1UL, 2UL, 3UL, 4UL));
    CHECK(value_called == 4);

    auto new_forcing = forcing_factory();
    new_forcing.apply_forces(mock_time, mock_system, mock_index);
    CHECK(value_called == 5);
    CHECK(std::addressof(new_forcing) != std::addressof(my_forcing));
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.MakeForcing.implementation",
                     "[Unit][Simulator][Forcing]") {
    // These always create a copy of the functors in question
    test_make_forcing_for_lvalue_references();
    test_make_forcing_for_prvalues();
    test_make_forcing_for_xvalues();
  }

  struct ANC : elastica::NonCopyable {
    template <typename TimeType, typename SystemType, typename IndexType,
              typename... Args>
    void operator()(TimeType, SystemType& s, IndexType, Args...) const {
      ++s;
    }
    mutable int times_called = {0};
  };

  void test_make_forcing_for_noncopyable_values() {
    auto a{ANC()};
    auto forcing_factory = make_forcing_detail::make_forcing(
        std::move(a), [](auto, auto, auto, auto...) {});
    // now make a constrint
    auto my_forcing = forcing_factory();
    // forcing interface
    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;
    my_forcing.apply_forces(mock_time, mock_system, mock_index);
    CHECK(mock_system == 3);
  }

  void test_make_forcing_for_noncopyable_rates() {
    auto a{ANC()};
    auto forcing_factory = make_forcing_detail::make_forcing(
        [](auto, auto, auto, auto...) {}, std::move(a));
    // now make a constrint
    auto my_forcing = forcing_factory();
    // forcing interface
    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;
    my_forcing.apply_torques(mock_time, mock_system, mock_index);
    CHECK(mock_system == 3);
  }

  void test_make_forcing_for_both_noncopyables() {
    auto a1{ANC()};
    auto a2{ANC()};
    auto forcing_factory =
        make_forcing_detail::make_forcing(std::move(a1), std::move(a2));
    // now make a constrint
    auto my_forcing = forcing_factory();
    // forcing interface
    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;
    my_forcing.apply_forces(mock_time, mock_system, mock_index);
    CHECK(mock_system == 3);
    my_forcing.apply_torques(mock_time, mock_system, mock_index);
    CHECK(mock_system == 4);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.MakeForcing.noncopyables",
                     "[Unit][Simulator][Forcing]") {
    // These always create a copy of the functors in question
    test_make_forcing_for_noncopyable_values();
    test_make_forcing_for_noncopyable_rates();
    test_make_forcing_for_both_noncopyables();
  }

  void test_make_forcing_throws_for_noncopyable_values() {
    auto a{ANC()};
    auto forcing_factory = make_forcing_detail::make_forcing(
        std::move(a), [](auto, auto, auto, auto...) {});
    auto fc = forcing_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(forcing_factory(),
                      Catch::Contains("the first functor is non-copyable"));
  }

  void test_make_forcing_throws_for_noncopyable_rates() {
    auto a{ANC()};
    auto forcing_factory = make_forcing_detail::make_forcing(
        [](auto, auto, auto, auto...) {}, std::move(a));
    auto fc = forcing_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(forcing_factory(),
                      Catch::Contains("the second functor is non-copyable"));
  }

  void test_make_forcing_throws_for_both_noncopyables() {
    auto a1{ANC()};
    auto a2{ANC()};
    auto forcing_factory =
        make_forcing_detail::make_forcing(std::move(a1), std::move(a2));
    auto fc = forcing_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(forcing_factory(),
                      Catch::Contains("both the functors are non-copyable"));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Forcing.MakeForcing.throws_for_repeated_use_of_noncopyables",
      "[Unit][Simulator][Forcing]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_forcing_throws_for_noncopyable_values();
    test_make_forcing_throws_for_noncopyable_rates();
    test_make_forcing_throws_for_both_noncopyables();
  }

  struct CountedA : counter<CountedA> {
    template <typename TimeType, typename SystemType, typename IndexType>
    void operator()(TimeType, SystemType&, IndexType) const {}
  };

  struct CountedB : counter<CountedB> {
    explicit CountedB(int g) : times_called(g){};
    template <typename TimeType, typename SystemType, typename IndexType>
    void operator()(TimeType, SystemType&, IndexType) const {
      ++times_called;
    }
    mutable int times_called;
  };

  IGNORE_UNUSED
  void test_make_forcing_statistics_for_both_lvalue_references() {
    auto a{CountedA()};   // 1 created
    auto b{CountedB(2)};  // 1

    CHECK(counter<CountedA>::objects_alive == 1);  // 1 object is a
    CHECK(counter<CountedA>::objects_copied == 0);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 1);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 0);
    CHECK(counter<CountedB>::objects_moved == 0);

    // a and b are captured by references inside, and copies are made
    // in forcing_factory upon invocation
    auto forcing_factory = make_forcing_detail::make_forcing(a, b);

    CHECK(counter<CountedA>::objects_alive == 1);  // 1 object is a
    CHECK(counter<CountedA>::objects_copied == 0);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 1);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 0);
    CHECK(counter<CountedB>::objects_moved == 0);

    // now make a forcing
    auto my_forcing = forcing_factory();

    CHECK(counter<CountedA>::objects_alive == 2);
    CHECK(counter<CountedA>::objects_copied == 1);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 2);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 1);
    CHECK(counter<CountedB>::objects_moved == 0);
    elastica::ignore_unused(my_forcing);
  }

  void test_make_forcing_interface() {
    auto a{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // xvalues and prvalues

      /// [make_torque_example]
      auto forcing_factory = make_forcing(
          make_force(std::move(a)),
          make_torque([](auto /*time*/, auto& /*system*/, auto /*index*/) {}));
      /// [make_torque_example]

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);
    }

    {  // xvalues and prvalues

      /// [make_force_example]
      auto forcing_factory = make_forcing(
          make_torque(std::move(c)),
          make_force([](auto /*time*/, auto& /*system*/, auto /*index*/) {}));
      /// [make_force_example]

      // now make a constrint
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);
    }

    {  // lvalues
      auto forcing_factory = make_forcing(make_force(b), make_torque(b));
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_torques(mock_time, mock_system, mock_index);
      // referece update
      CHECK(b.times_called == 1);
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      // referece update
      CHECK(b.times_called == 2);
    }

    {
      /// [make_forcing_example]
      // This makes a forcing with two functors
      // however passing the functors directly is not advised and so forbidden
      // rather pass them through make_force() and
      // make_torque(), to clearly highlight which function is which
      // for the reader
      auto forcing_factory = make_forcing(
          make_force([](auto /*time*/, auto& /*system*/, auto /*index*/) {}),
          make_torque([](auto /*time*/, auto& /*system*/, auto /*index*/) {}));

      // this only makes a class
      // we now need to instantiate that class to use it
      // and we do that by calling the factory's operator() method

      auto my_forcing = forcing_factory();

      // now we can use my_forcing(), which has the interface of a
      // elastica forcing object:

      // my_forcing.apply_forces(...);
      // my_forcing.apply_torques(...);

      // Additionally, order of make_force and make_torque don't matter
      auto another_forcing_factory = make_forcing(
          make_torque([](auto /*time*/, auto& /*system*/, auto /*index*/) {}),
          make_force([](auto /*time*/, auto& /*system*/, auto /*index*/) {}));

      /// [make_forcing_example]
      elastica::ignore_unused(my_forcing);
      elastica::ignore_unused(another_forcing_factory);
    }
  }

  void test_make_forcing_interface_parametrized() {
    // two ways of doing it

    // cleaner, but requires programmer to know basic C++
    /// [make_parametrized_forcing_example]
    auto forcing_factory = [](int x, int y) {
      return make_forcing(
          make_force([=](auto /*time*/, auto& system, auto /*index*/
                     ) { system += x; }),
          make_torque([=](auto /*time*/, auto& system, auto /*index*/) {
            system += y;
          }))();
    };
    // The above forcing is parameterized on x and y
    // and this is its usage

    // auto my_forcing = forcing_factory(2, 1);

    // now you can use my_forcing similar to the other example!
    /// [make_parametrized_forcing_example]

    /*
    // much more confusing, requires programmer to return a lambda within a
    // lambda, also repeating syntax
    auto new_forcing_factory = [](int x, int y) {
      return make_forcing(
          make_torque([](int x, int y) {
            return [x](auto& sys, auto, auto, auto...) { sys += x; };
          }),
          make_force([](int x, int y) {
            return [y](auto& sys, auto, auto, auto...) { sys += y; };
          }));
    };
    */

    {  // lvalues
      auto my_forcing = forcing_factory(2, 1);
      // forcing interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      // referece update
      CHECK(mock_system == 4);
      // referece update
      my_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(mock_system == 5);

      auto new_forcing = forcing_factory(2, 1);
      new_forcing.apply_forces(mock_time, mock_system, mock_index);
      // referece update
      CHECK(mock_system == 7);
      // referece update
      new_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(mock_system == 8);

      auto another_forcing = forcing_factory(10, 20);
      another_forcing.apply_forces(mock_time, mock_system, mock_index);
      // referece update
      CHECK(mock_system == 18);
      // referece update
      another_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(mock_system == 38);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Forcing.MakeForcing.interface",
      "[Unit][Simulator][Forcing]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_forcing_interface();
    test_make_forcing_interface_parametrized();
  }

  void test_make_forcing_overloaded_forces_interface() {
    auto temp1{ANC()};
    auto temp2{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // prforces and xforces

      // First test with only two, then do more
      auto forcing_factory = make_forcing(
          make_force(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              }),
          make_torque(std::move(temp1)));

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_forcing.apply_forces(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.3F));
    }

    {
      // First test with only two, then do more
      /// [make_force_overloaded_example]
      auto forcing_factory = make_forcing(
          make_force(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              },
              [](auto /*time*/, double& system, auto /*index*/) {
                system -= 0.8;
              },
              [](auto /*time*/, char& system, auto /*index*/) { system = 'b'; },

              [](auto /*time*/, auto& /*system*/, auto /*index*/) {
                // no-operation as a default fallback for all other systems,
                // can be skipped so as to generate compile-time errors in case
                // your custom forcing is not meant to be mapped to arbitrary
                // systems!
              }),
          make_torque(std::move(temp2)));
      /// [make_force_overloaded_example]

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_forcing.apply_forces(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.3F));

      double mock_double_system = 0.2;
      my_forcing.apply_forces(mock_time, mock_double_system, mock_index);
      CHECK(mock_double_system == approx(-0.6));

      char mock_char_system = 'p';
      my_forcing.apply_forces(mock_time, mock_char_system, mock_index);
      CHECK(mock_char_system == 'b');

      // no-operation for bool type say
      bool mock_bool_system = true;
      my_forcing.apply_forces(mock_time, mock_bool_system, mock_index);
      CHECK(mock_bool_system);
    }

    {  // xforces and prforces

      auto fover = [](auto /*time*/, float& system, auto /*index*/) {
        system -= 0.5F;
      };

      auto forcing_factory = make_forcing(
          make_torque(std::move(c)),
          make_force(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              std::move(fover)));

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);

      float mock_float_system(1.5F);
      my_forcing.apply_forces(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*index*/) {
        system += 1.5;
      };

      auto cover = [](auto /*time*/, char& system, auto /*index*/) {
        system = 'b';
      };

      auto new_forcing_factory = make_forcing(
          make_torque(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; }),
          make_force(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              std::move(dover), std::move(cover),
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              }));

      // now make a new_forcing
      auto my_new_forcing = new_forcing_factory();
      // new_forcing interface
      CHECK(mock_system == 3);
      my_new_forcing.apply_forces(mock_time, mock_system, mock_index);
      CHECK(mock_system == 4);

      my_new_forcing.apply_forces(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.5F));
    }

    {  // lforces

      auto fover = [](auto /*time*/, float& system, auto /*index*/) {
        system -= 0.5F;
      };
      const auto iover = [](auto /*time*/, int& system, auto /*index*/) {
        system += 1;
      };

      auto forcing_factory =
          make_forcing(make_force(fover, iover), make_torque(b));

      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_torques(mock_time, mock_int_system, mock_index);
      // referece update
      CHECK(b.times_called == 1);

      CHECK(mock_int_system == 2);
      my_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system(1.5F);
      my_forcing.apply_forces(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      const auto dover = [](auto /*time*/, double& system, auto /*index*/) {
        system += 1.5;
      };

      auto cover = [](auto /*time*/, char& system, auto /*index*/) {
        system = 'b';
      };

      auto new_forcing_factory =
          make_forcing(make_force(fover, iover, dover, cover), make_torque(b));

      auto my_new_forcing = new_forcing_factory();
      // forcing interface
      my_new_forcing.apply_torques(mock_time, mock_int_system, mock_index);
      // referece update
      CHECK(b.times_called == 2);

      CHECK(mock_int_system == 3);
      my_new_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 4);

      my_new_forcing.apply_forces(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.5F));

      double mock_double_system = 0.1;
      my_new_forcing.apply_forces(mock_time, mock_double_system, mock_index);
      CHECK(mock_double_system == approx(1.6));

      char mock_char_system = 'p';
      my_new_forcing.apply_forces(mock_time, mock_char_system, mock_index);
      CHECK(mock_char_system == 'b');
    }
  }

  void test_make_forcing_overloaded_torques_interface() {
    auto temp1{ANC()};
    auto temp2{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // prtorques and torquesx

      // First test with only two, then do more
      auto forcing_factory = make_forcing(
          make_torque(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              }),
          make_force(std::move(temp1)));

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_torques(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_forcing.apply_torques(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.3F));
    }

    {
      // First test with only two, then do more
      /// [make_torque_overloaded_example]
      auto forcing_factory = make_forcing(
          make_torque(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              },
              [](auto /*time*/, double& system, auto /*index*/) {
                system -= 0.8;
              },
              [](auto /*time*/, char& system, auto /*index*/) { system = 'b'; },
              [](auto /*time*/, auto& /*system*/, auto /*index*/) {
                // no-operation as a default fallback for all other systems,
                // can be skipped so as to generate compile-time errors in case
                // your custom forcing is not meant to be mapped to arbitrary
                // systems!
              }),
          make_force(std::move(temp2)));
      /// [make_torque_overloaded_example]

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_torques(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_forcing.apply_torques(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.3F));

      double mock_double_system = 0.2;
      my_forcing.apply_torques(mock_time, mock_double_system, mock_index);
      CHECK(mock_double_system == approx(-0.6));

      char mock_char_system = 'p';
      my_forcing.apply_torques(mock_time, mock_char_system, mock_index);
      CHECK(mock_char_system == 'b');

      // no-operation for bool type say
      bool mock_bool_system = true;
      my_forcing.apply_torques(mock_time, mock_bool_system, mock_index);
      CHECK(mock_bool_system);
    }

    {  // torquesx and prtorques

      auto fover = [](auto /*time*/, float& system, auto /*index*/) {
        system -= 0.5F;
      };

      auto forcing_factory = make_forcing(
          make_force(std::move(c)),
          make_torque(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              std::move(fover)));

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);

      float mock_float_system(1.5F);
      my_forcing.apply_torques(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*index*/) {
        system += 1.5;
      };

      auto cover = [](auto /*time*/, char& system, auto /*index*/) {
        system = 'b';
      };

      auto new_forcing_factory = make_forcing(
          make_force(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; }),
          make_torque(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              std::move(dover), std::move(cover),
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              }));

      // now make a new_forcing
      auto my_new_forcing = new_forcing_factory();
      // new_forcing interface
      CHECK(mock_system == 3);
      my_new_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(mock_system == 4);

      my_new_forcing.apply_torques(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.5F));
    }

    {  // ltorques

      auto fover = [](auto /*time*/, float& system, auto /*index*/) {
        system -= 0.5F;
      };
      auto iover = [](auto /*time*/, int& system, auto /*index*/) {
        system += 1;
      };

      auto forcing_factory =
          make_forcing(make_torque(fover, iover), make_force(b));

      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      // referece update
      CHECK(b.times_called == 1);

      CHECK(mock_int_system == 2);
      my_forcing.apply_torques(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system(1.5F);
      my_forcing.apply_torques(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*index*/) {
        system += 1.5;
      };

      auto cover = [](auto /*time*/, char& system, auto /*index*/) {
        system = 'b';
      };

      auto new_forcing_factory =
          make_forcing(make_torque(fover, iover, dover, cover), make_force(b));

      auto my_new_forcing = new_forcing_factory();
      // forcing interface
      my_new_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      // referece update
      CHECK(b.times_called == 2);

      CHECK(mock_int_system == 3);
      my_new_forcing.apply_torques(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 4);

      my_new_forcing.apply_torques(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.5F));

      double mock_double_system = 0.1;
      my_new_forcing.apply_torques(mock_time, mock_double_system, mock_index);
      CHECK(mock_double_system == approx(1.6));

      char mock_char_system = 'p';
      my_new_forcing.apply_torques(mock_time, mock_char_system, mock_index);
      CHECK(mock_char_system == 'b');
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Forcing.MakeForcing.overloaded_interface",
      "[Unit][Simulator][Forcing]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_forcing_overloaded_forces_interface();
    test_make_forcing_overloaded_torques_interface();
  }

  void test_make_forcing_unified_interface() {
    SECTION("Unified interface") {
      /// [make_unified_example]
      auto forcing_factory = make_forcing(ANC());
      /// [make_unified_example]

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);
    }
    SECTION("Unified interface with overloading") {
      /// [make_unified_overloading_eg]
      const auto fover = [](auto /*time*/, float& system, auto /*first_index*/
                         ) { system -= 0.5F; };
      auto iover = [](auto /*time*/, int& system, auto /*first_index*/) {
        system += 1;
      };
      auto forcing_factory = make_forcing(fover, iover);
      /// [make_unified_overloading_eg]

      // now make a forcing
      auto my_forcing = forcing_factory();
      // forcing interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_forcing.apply_forces(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.3F));
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.MakeForcing.unified_interface",
                     "[Unit][Simulator][Forcing]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_forcing_unified_interface();
  }
}  // namespace
