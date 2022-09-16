//
// Created by Tejaswin Parthasarathy on 1/3/21.
//

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Constraints/MakeConstraint.hpp"
#include "Simulator/Constraints/Protocols.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {
  using namespace elastica::constraints;
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

  void test_make_constraint_for_lvalue_references() {
    auto a{A()};  // 1 created
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1

    // a and b are captured by references inside, and copies are made
    // in constraint_factory upon invocation
    auto constraint_factory = make_constraint_detail::make_constraint(a, b);
    // do static assert once for all make_forcing
    static_assert(::tt::assert_conforms_to<decltype(constraint_factory()),
                                           protocols::Constraint>,
                  "Protocol violation!");

    // now make a constraint
    auto my_constraint = constraint_factory();

    // check priori
    CHECK(a.times_called == 0);
    CHECK(b.times_called == 0);

    // constraint interface
    int mock_time = 2;
    int mock_system = 3;

    // calls a 6 times
    my_constraint.constrain_primitives_internal(
        mock_time, mock_system, numeric_at(1UL, 2UL, 3UL, 4UL, 5UL, 6UL));
    // calls B 3 times
    my_constraint.constrain_rates_internal(mock_time, mock_system,
                                           numeric_at(1UL, 2UL, 3UL));

    // check the number of times the functor is called
    // will be 0 because it is copied as expected
    CHECK(a.times_called == 0);
    // but b had a reference and it is updated there
    CHECK(b.times_called == 3);

    my_constraint.constrain_primitives(mock_time, mock_system, 1);
    my_constraint.constrain_rates(mock_time, mock_system, 3);

    CHECK(b.times_called == 4);

    auto new_constraint = constraint_factory();
    CHECK(std::addressof(new_constraint) != std::addressof(my_constraint));
  }

  struct AggregateB {
    explicit AggregateB(std::string s) : s_(std::move(s)){};
    template <typename TimeType, typename SystemType, typename IndexType>
    void operator()(TimeType, SystemType& s, IndexType) const {
      ++s;
    }
    std::string s_;
  };

  void test_make_constraint_for_xprimitives() {
    auto b1{AggregateB("Melt")};
    auto b2{AggregateB("Gibsont")};

    auto constraint_factory =
        make_constraint_detail::make_constraint(std::move(b1), std::move(b2));

    // now make a constraint
    auto my_constraint = constraint_factory();

    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;

    my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
    CHECK(mock_system == 3);
    my_constraint.constrain_rates(mock_time, mock_system, mock_index);
    CHECK(mock_system == 4);

    auto new_constraint = constraint_factory();
    CHECK(std::addressof(new_constraint) != std::addressof(my_constraint));
  }

  void test_make_constraint_for_prprimitives() {
    // a and b are captured by references inside, and copies are made
    // in constraint_factory upon invocation
    int value_called = 0;
    auto constraint_factory = make_constraint_detail::make_constraint(
        [&value_called](auto, auto, auto) { ++value_called; },
        [](auto, auto, auto) {});

    // now make a constraint
    auto my_constraint = constraint_factory();
    // constraint interface
    int mock_time = 2;
    int mock_system = 3;
    std::size_t mock_index = 1;
    my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
    CHECK(value_called == 1);
    value_called = 0;
    my_constraint.constrain_primitives_internal(mock_time, mock_system,
                                                numeric_at(1UL, 2UL, 3UL, 4UL));
    CHECK(value_called == 4);

    auto new_constraint = constraint_factory();
    new_constraint.constrain_primitives(mock_time, mock_system, mock_index);
    CHECK(value_called == 5);
    CHECK(std::addressof(new_constraint) != std::addressof(my_constraint));
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Constraints.MakeConstraint.implementation",
                     "[Unit][Simulator][Constraints]") {
    // These always create a copy of the functors in question
    test_make_constraint_for_lvalue_references();
    test_make_constraint_for_prprimitives();
    test_make_constraint_for_xprimitives();
  }

  struct ANC : elastica::NonCopyable {
    template <typename TimeType, typename SystemType, typename IndexType,
              typename... Args>
    void operator()(TimeType, SystemType& s, IndexType, Args...) const {
      ++s;
    }
    mutable int times_called = {0};
  };

  void test_make_constraint_for_noncopyable_primitives() {
    auto a{ANC()};
    auto constraint_factory = make_constraint_detail::make_constraint(
        std::move(a), [](auto, auto, auto, auto...) {});
    // now make a constraint
    auto my_constraint = constraint_factory();
    // constraint interface
    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;
    my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
    CHECK(mock_system == 3);
  }

  void test_make_constraint_for_noncopyable_rates() {
    auto a{ANC()};
    auto constraint_factory = make_constraint_detail::make_constraint(
        [](auto, auto, auto, auto...) {}, std::move(a));
    // now make a constraint
    auto my_constraint = constraint_factory();
    // constraint interface
    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;
    my_constraint.constrain_rates(mock_time, mock_system, mock_index);
    CHECK(mock_system == 3);
  }

  void test_make_constraint_for_both_noncopyables() {
    auto a1{ANC()};
    auto a2{ANC()};
    auto constraint_factory =
        make_constraint_detail::make_constraint(std::move(a1), std::move(a2));
    // now make a constraint
    auto my_constraint = constraint_factory();
    // constraint interface
    int mock_time = 3;
    int mock_system = 2;
    std::size_t mock_index = 1;
    my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
    CHECK(mock_system == 3);
    my_constraint.constrain_rates(mock_time, mock_system, mock_index);
    CHECK(mock_system == 4);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Constraints.MakeConstraint.noncopyables",
                     "[Unit][Simulator][Constraints]") {
    // These always create a copy of the functors in question
    test_make_constraint_for_noncopyable_primitives();
    test_make_constraint_for_noncopyable_rates();
    test_make_constraint_for_both_noncopyables();
  }

  void test_make_constraint_throws_for_noncopyable_primitives() {
    auto a{ANC()};
    auto constraint_factory = make_constraint_detail::make_constraint(
        std::move(a), [](auto, auto, auto, auto...) {});
    auto fc = constraint_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(constraint_factory(),
                      Catch::Contains("the first functor is non-copyable"));
  }

  void test_make_constraint_throws_for_noncopyable_rates() {
    auto a{ANC()};
    auto constraint_factory = make_constraint_detail::make_constraint(
        [](auto, auto, auto, auto...) {}, std::move(a));
    auto fc = constraint_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(constraint_factory(),
                      Catch::Contains("the second functor is non-copyable"));
  }

  void test_make_constraint_throws_for_both_noncopyables() {
    auto a1{ANC()};
    auto a2{ANC()};
    auto constraint_factory =
        make_constraint_detail::make_constraint(std::move(a1), std::move(a2));
    auto fc = constraint_factory();
    elastica::ignore_unused(fc);
    CHECK_THROWS_WITH(constraint_factory(),
                      Catch::Contains("both the functors are non-copyable"));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.MakeConstraint.throws_for_repeated_use_of_noncopyables",
      "[Unit][Simulator][Constraints]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_constraint_throws_for_noncopyable_primitives();
    test_make_constraint_throws_for_noncopyable_rates();
    test_make_constraint_throws_for_both_noncopyables();
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
  void test_make_constraint_statistics_for_both_lvalue_references() {
    auto a{CountedA()};   // 1 created
    auto b{CountedB(2)};  // 1

    CHECK(counter<CountedA>::objects_alive == 1);  // 1 object is a
    CHECK(counter<CountedA>::objects_copied == 0);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 1);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 0);
    CHECK(counter<CountedB>::objects_moved == 0);

    // a and b are captured by references inside, and copies are made
    // in constraint_factory upon invocation
    auto constraint_factory = make_constraint_detail::make_constraint(a, b);

    CHECK(counter<CountedA>::objects_alive == 1);  // 1 object is a
    CHECK(counter<CountedA>::objects_copied == 0);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 1);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 0);
    CHECK(counter<CountedB>::objects_moved == 0);

    // now make a constraint
    auto my_constraint = constraint_factory();

    CHECK(counter<CountedA>::objects_alive == 2);
    CHECK(counter<CountedA>::objects_copied == 1);
    CHECK(counter<CountedA>::objects_moved == 0);

    CHECK(counter<CountedB>::objects_alive == 2);  // 1 object is b
    CHECK(counter<CountedB>::objects_copied == 1);
    CHECK(counter<CountedB>::objects_moved == 0);

    elastica::ignore_unused(my_constraint);
  }

  void test_make_constraint_interface() {
    auto a{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // xprimitives and prprimitives

      /// [make_rate_constraint_example]
      auto constraint_factory = make_constraint(
          make_value_constraint(std::move(a)),
          make_rate_constraint(
              [](auto /*time*/, auto& /*system*/, auto /*index*/) {}));
      /// [make_rate_constraint_example]

      // now make a constraint
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);
    }

    {  // xprimitives and prprimitives

      /// [make_value_constraint_example]
      auto constraint_factory = make_constraint(
          make_rate_constraint(std::move(c)),
          make_value_constraint(
              [](auto /*time*/, auto& /*system*/, auto /*index*/) {}));
      /// [make_value_constraint_example]

      // now make a constraint
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_rates(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);
    }

    {  // lprimitives
      auto constraint_factory =
          make_constraint(make_value_constraint(b), make_rate_constraint(b));
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_rates(mock_time, mock_system, mock_index);
      // referece update
      CHECK(b.times_called == 1);
      my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
      // referece update
      CHECK(b.times_called == 2);
    }

    {
      /// [make_constraint_example]
      // This makes a constraint with two functors
      // however passing the functors directly is not advised and so forbidden
      // rather pass them through make_value_constraint() and
      // make_rate_constraint(), to clearly highlight which function is which
      // for the reader
      auto constraint_factory = make_constraint(
          make_value_constraint(
              [](auto /*time*/, auto& /*system*/, auto /*index*/) {}),
          make_rate_constraint(
              [](auto /*time*/, auto& /*system*/, auto /*index*/) {}));

      // this only makes a class
      // we now need to instantiate that class to use it
      // and we do that by calling the factory's operator() method

      auto my_constraint = constraint_factory();

      // now we can use my_constraint(), which has the interface of a
      // elastica constraint object:

      // my_constraint.constrain_primitives(...);
      // my_constraint.constrain_rates(...);

      // Additionally, order of make_value_constraint and make_rate_constraint
      // don't matter
      auto another_forcing_factory = make_constraint(
          make_value_constraint(
              [](auto /*time*/, auto& /*system*/, auto /*index*/) {}),
          make_rate_constraint(
              [](auto /*time*/, auto& /*system*/, auto /*index*/) {}));

      /// [make_constraint_example]
      elastica::ignore_unused(my_constraint);
      elastica::ignore_unused(another_forcing_factory);
    }
  }

  void test_make_constraint_interface_parametrized() {
    // two ways of doing it

    // cleaner, but requires programmer to know basic C++
    /// [make_parametrized_constraint_example]
    auto constraint_factory = [](int x, int y) {
      return make_constraint(
          make_value_constraint([=](auto /*time*/, auto& system,
                                    auto /*index*/) { system += x; }),
          make_rate_constraint([=](auto /*time*/, auto& system,
                                   auto /*index*/) { system += y; }))();
    };
    // The above constraint is parameterized on x and y
    // and this is its usage

    // auto my_constraint = constraint_factory(2, 1);

    // now you can use my_constraint similar to the other example!
    /// [make_parametrized_constraint_example]

    /*
    // much more confusing, requires programmer to return a lambda within a
    // lambda, also repeating syntax
    auto new_constraint_factory = [](int x, int y) {
      return make_constraint(
          make_rate_constraint([](int x, int y) {
            return [x](auto& sys, auto, auto, auto...) { sys += x; };
          }),
          make_value_constraint([](int x, int y) {
            return [y](auto& sys, auto, auto, auto...) { sys += y; };
          }));
    };
    */

    {  // lprimitives
      auto my_constraint = constraint_factory(2, 1);
      // constraint interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
      // referece update
      CHECK(mock_system == 4);
      // referece update
      my_constraint.constrain_rates(mock_time, mock_system, mock_index);
      CHECK(mock_system == 5);

      auto new_constraint = constraint_factory(2, 1);
      new_constraint.constrain_primitives(mock_time, mock_system, mock_index);
      // referece update
      CHECK(mock_system == 7);
      // referece update
      new_constraint.constrain_rates(mock_time, mock_system, mock_index);
      CHECK(mock_system == 8);

      auto another_constraint = constraint_factory(10, 20);
      another_constraint.constrain_primitives(mock_time, mock_system,
                                              mock_index);
      // referece update
      CHECK(mock_system == 18);
      // referece update
      another_constraint.constrain_rates(mock_time, mock_system, mock_index);
      CHECK(mock_system == 38);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.MakeConstraint.interface",
      "[Unit][Simulator][Constraints]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_constraint_interface();
    test_make_constraint_interface_parametrized();
  }

  void test_make_constraint_overloaded_primitives_interface() {
    auto temp1{ANC()};
    auto temp2{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // prprimitives and xprimitives

      // First test with only two, then do more
      auto constraint_factory = make_constraint(
          make_value_constraint(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              }),
          make_rate_constraint(std::move(temp1)));

      // now make a constraint
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_primitives(mock_time, mock_int_system,
                                         mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_constraint.constrain_primitives(mock_time, mock_float_system,
                                         mock_index);
      CHECK(mock_float_system == approx(0.3F));
    }

    {
      // First test with only two, then do more
      /// [make_value_constraint_overloaded_example]
      auto constraint_factory = make_constraint(
          make_value_constraint(
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
                // your custom constraint is not meant to be mapped to arbitrary
                // systems!
              }),
          make_rate_constraint(std::move(temp2)));
      /// [make_value_constraint_overloaded_example]

      // now make a constraint
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_primitives(mock_time, mock_int_system,
                                         mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_constraint.constrain_primitives(mock_time, mock_float_system,
                                         mock_index);
      CHECK(mock_float_system == approx(0.3F));

      double mock_double_system = 0.2;
      my_constraint.constrain_primitives(mock_time, mock_double_system,
                                         mock_index);
      CHECK(mock_double_system == approx(-0.6));

      char mock_char_system = 'p';
      my_constraint.constrain_primitives(mock_time, mock_char_system,
                                         mock_index);
      CHECK(mock_char_system == 'b');

      // no-operation for bool type say
      bool mock_bool_system = true;
      my_constraint.constrain_primitives(mock_time, mock_bool_system,
                                         mock_index);
      CHECK(mock_bool_system);
    }

    {  // xprimitives and prprimitives

      auto fover = [](auto /*time*/, float& system, auto /*index*/) {
        system -= 0.5F;
      };

      auto constraint_factory = make_constraint(
          make_rate_constraint(std::move(c)),
          make_value_constraint(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              std::move(fover)));

      // now make a constraint
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);

      float mock_float_system(1.5F);
      my_constraint.constrain_primitives(mock_time, mock_float_system,
                                         mock_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*index*/) {
        system += 1.5;
      };

      auto cover = [](auto /*time*/, char& system, auto /*index*/) {
        system = 'b';
      };

      auto new_constraint_factory = make_constraint(
          make_rate_constraint(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; }),
          make_value_constraint(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              std::move(dover), std::move(cover),
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              }));

      // now make a new_constraint
      auto my_new_constraint = new_constraint_factory();
      // new_constraint interface
      CHECK(mock_system == 3);
      my_new_constraint.constrain_primitives(mock_time, mock_system,
                                             mock_index);
      CHECK(mock_system == 4);

      my_new_constraint.constrain_primitives(mock_time, mock_float_system,
                                             mock_index);
      CHECK(mock_float_system == approx(0.5F));
    }

    {  // lprimitives

      auto fover = [](auto /*time*/, float& system, auto /*index*/) {
        system -= 0.5F;
      };
      auto iover = [](auto /*time*/, int& system, auto /*index*/) {
        system += 1;
      };

      auto constraint_factory = make_constraint(
          make_value_constraint(fover, iover), make_rate_constraint(b));

      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_rates(mock_time, mock_int_system, mock_index);
      // referece update
      CHECK(b.times_called == 1);

      CHECK(mock_int_system == 2);
      my_constraint.constrain_primitives(mock_time, mock_int_system,
                                         mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system(1.5F);
      my_constraint.constrain_primitives(mock_time, mock_float_system,
                                         mock_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*index*/) {
        system += 1.5;
      };

      auto cover = [](auto /*time*/, char& system, auto /*index*/) {
        system = 'b';
      };

      auto new_constraint_factory =
          make_constraint(make_value_constraint(fover, iover, dover, cover),
                          make_rate_constraint(b));

      auto my_new_constraint = new_constraint_factory();
      // constraint interface
      my_new_constraint.constrain_rates(mock_time, mock_int_system, mock_index);
      // referece update
      CHECK(b.times_called == 2);

      CHECK(mock_int_system == 3);
      my_new_constraint.constrain_primitives(mock_time, mock_int_system,
                                             mock_index);
      CHECK(mock_int_system == 4);

      my_new_constraint.constrain_primitives(mock_time, mock_float_system,
                                             mock_index);
      CHECK(mock_float_system == approx(0.5F));

      double mock_double_system = 0.1;
      my_new_constraint.constrain_primitives(mock_time, mock_double_system,
                                             mock_index);
      CHECK(mock_double_system == approx(1.6));

      char mock_char_system = 'p';
      my_new_constraint.constrain_primitives(mock_time, mock_char_system,
                                             mock_index);
      CHECK(mock_char_system == 'b');
    }
  }

  void test_make_constraint_overloaded_rates_interface() {
    auto temp1{ANC()};
    auto temp2{ANC()};
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1
    auto c{ANC()};

    {  // prrates and ratesx

      // First test with only two, then do more
      auto constraint_factory = make_constraint(
          make_rate_constraint(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              }),
          make_value_constraint(std::move(temp1)));

      // now make a constraint
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_rates(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_constraint.constrain_rates(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.3F));
    }

    {
      // First test with only two, then do more
      /// [make_rate_constraint_overloaded_example]
      auto constraint_factory = make_constraint(
          make_rate_constraint(
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
                // your custom constraint is not meant to be mapped to arbitrary
                // systems!
              }),
          make_value_constraint(std::move(temp2)));
      /// [make_rate_constraint_overloaded_example]

      // now make a constraint
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_rates(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system = 0.8F;
      my_constraint.constrain_rates(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(0.3F));

      double mock_double_system = 0.2;
      my_constraint.constrain_rates(mock_time, mock_double_system, mock_index);
      CHECK(mock_double_system == approx(-0.6));

      char mock_char_system = 'p';
      my_constraint.constrain_rates(mock_time, mock_char_system, mock_index);
      CHECK(mock_char_system == 'b');

      // no-operation for bool type say
      bool mock_bool_system = true;
      my_constraint.constrain_rates(mock_time, mock_bool_system, mock_index);
      CHECK(mock_bool_system);
    }

    {  // ratesx and prrates

      auto fover = [](auto /*time*/, float& system, auto /*index*/) {
        system -= 0.5F;
      };

      auto constraint_factory = make_constraint(
          make_value_constraint(std::move(c)),
          make_rate_constraint(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              std::move(fover)));

      // now make a constraint
      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_rates(mock_time, mock_system, mock_index);
      CHECK(mock_system == 3);

      float mock_float_system(1.5F);
      my_constraint.constrain_rates(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*index*/) {
        system += 1.5;
      };

      auto cover = [](auto /*time*/, char& system, auto /*index*/) {
        system = 'b';
      };

      auto new_constraint_factory = make_constraint(
          make_value_constraint(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; }),
          make_rate_constraint(
              [](auto /*time*/, int& system, auto /*index*/) { system += 1; },
              std::move(dover), std::move(cover),
              [](auto /*time*/, float& system, auto /*index*/) {
                system -= 0.5F;
              }));

      // now make a new_constraint
      auto my_new_constraint = new_constraint_factory();
      // new_constraint interface
      CHECK(mock_system == 3);
      my_new_constraint.constrain_rates(mock_time, mock_system, mock_index);
      CHECK(mock_system == 4);

      my_new_constraint.constrain_rates(mock_time, mock_float_system,
                                        mock_index);
      CHECK(mock_float_system == approx(0.5F));
    }

    {  // lrates

      auto fover = [](auto /*time*/, float& system, auto /*index*/) {
        system -= 0.5F;
      };
      auto iover = [](auto /*time*/, int& system, auto /*index*/) {
        system += 1;
      };

      auto constraint_factory = make_constraint(
          make_rate_constraint(fover, iover), make_value_constraint(b));

      auto my_constraint = constraint_factory();
      // constraint interface
      int mock_time = 3;
      int mock_int_system = 2;
      std::size_t mock_index = 1;
      my_constraint.constrain_primitives(mock_time, mock_int_system,
                                         mock_index);
      // referece update
      CHECK(b.times_called == 1);

      CHECK(mock_int_system == 2);
      my_constraint.constrain_rates(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 3);

      float mock_float_system(1.5F);
      my_constraint.constrain_rates(mock_time, mock_float_system, mock_index);
      CHECK(mock_float_system == approx(1.0F));

      // Test for more overloads
      auto dover = [](auto /*time*/, double& system, auto /*index*/) {
        system += 1.5;
      };

      auto cover = [](auto /*time*/, char& system, auto /*index*/) {
        system = 'b';
      };

      auto new_constraint_factory =
          make_constraint(make_rate_constraint(fover, iover, dover, cover),
                          make_value_constraint(b));

      auto my_new_constraint = new_constraint_factory();
      // constraint interface
      my_new_constraint.constrain_primitives(mock_time, mock_int_system,
                                             mock_index);
      // referece update
      CHECK(b.times_called == 2);

      CHECK(mock_int_system == 3);
      my_new_constraint.constrain_rates(mock_time, mock_int_system, mock_index);
      CHECK(mock_int_system == 4);

      my_new_constraint.constrain_rates(mock_time, mock_float_system,
                                        mock_index);
      CHECK(mock_float_system == approx(0.5F));

      double mock_double_system = 0.1;
      my_new_constraint.constrain_rates(mock_time, mock_double_system,
                                        mock_index);
      CHECK(mock_double_system == approx(1.6));

      char mock_char_system = 'p';
      my_new_constraint.constrain_rates(mock_time, mock_char_system,
                                        mock_index);
      CHECK(mock_char_system == 'b');
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.MakeConstraint.overloaded_interface",
      "[Unit][Simulator][Constraints]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_constraint_overloaded_primitives_interface();
    test_make_constraint_overloaded_rates_interface();
  }

}  // namespace
