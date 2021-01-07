//
// Created by Tejaswin Parthasarathy on 1/3/21.
//

#include <iostream>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Constraints/MakeConstraint.hpp"
#include "Simulator/Interface/At.hpp"
#include "Utilities/NonCopyable.hpp"

namespace {
  using namespace elastica::constraints;
  using namespace elastica::interface;

  struct A {
    template <typename S, typename T, typename I>
    void operator()(S&, T, I) const {
      ++times_called;
    }
    mutable int times_called = {0};
  };

  struct B {
    explicit B(int& g) : times_called(g){};
    template <typename S, typename T, typename I>
    void operator()(S&, T, I) const {
      ++times_called;
    }
    int& times_called;
  };

  void test_make_constraint_interface_for_lvalue_references() {
    auto a{A()};  // 1 created
    int n_times_b_called = 0;
    auto b{B(n_times_b_called)};  // 1

    // a and b are captured by references inside, and copies are made
    // in constraint_factory upon invocation
    auto constraint_factory = make_constraint(a, b);

    // now make a constrint
    auto my_constraint = constraint_factory();

    // check priori
    CHECK(a.times_called == 0);
    CHECK(b.times_called == 0);

    // constraint interface
    int mock_system = 2;
    int mock_time = 3;

    // calls a 6 times
    my_constraint.constrain_values_internal(mock_system, mock_time,
                                            at(1, 2, 3, 4, 5, 6));
    // calls B 3 times
    my_constraint.constrain_rates_internal(mock_system, mock_time, at(1, 2, 3));

    // check the number of times the functor is called
    // will be 0 because it is copied as expected
    CHECK(a.times_called == 0);
    // but b had a reference and it is updated there
    CHECK(b.times_called == 3);

    my_constraint.constrain_values(mock_system, mock_time, 1);
    my_constraint.constrain_rates(mock_system, mock_time, 3);

    CHECK(b.times_called == 4);

    auto new_constraint = constraint_factory();
    CHECK(std::addressof(new_constraint) != std::addressof(my_constraint));
  }

  struct AggregateB {
    explicit AggregateB(std::string s) : s_(std::move(s)){};
    template <typename S, typename T, typename I>
    void operator()(S& s, T, I) const {
      ++s;
    }
    std::string s_;
  };

  void test_make_constraint_interface_for_xvalues() {
    auto b1{AggregateB("Melt")};
    auto b2{AggregateB("Gibsont")};

    auto constraint_factory = make_constraint(std::move(b1), std::move(b2));

    // now make a constrint
    auto my_constraint = constraint_factory();

    int mock_system = 2;
    int mock_time = 3;
    int mock_index = 1;

    my_constraint.constrain_values(mock_system, mock_time, mock_index);
    CHECK(mock_system == 3);
    my_constraint.constrain_rates(mock_system, mock_time, mock_index);
    CHECK(mock_system == 4);

    auto new_constraint = constraint_factory();
    CHECK(std::addressof(new_constraint) != std::addressof(my_constraint));
  }

  void test_make_constraint_interface_for_prvalues() {
    // a and b are captured by references inside, and copies are made
    // in constraint_factory upon invocation
    int value_called = 0;
    auto constraint_factory =
        make_constraint([&value_called](auto, auto, auto) { ++value_called; },
                        [](auto, auto, auto) {});

    // now make a constrint
    auto my_constraint = constraint_factory();
    // constraint interface
    int mock_system = 2;
    int mock_time = 3;
    int mock_index = 1;
    my_constraint.constrain_values(mock_system, mock_time, mock_index);
    CHECK(value_called == 1);
    value_called = 0;
    my_constraint.constrain_values_internal(mock_system, mock_time,
                                            at(1, 2, 3, 4));
    CHECK(value_called == 4);

    auto new_constraint = constraint_factory();
    new_constraint.constrain_values(mock_system, mock_time, mock_index);
    CHECK(value_called == 5);
    CHECK(std::addressof(new_constraint) != std::addressof(my_constraint));
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Constraints.MakeConstraint.interface",
                     "[Unit][Simulator][Constraints]") {
    // These always create a copy of the functors in question
    test_make_constraint_interface_for_lvalue_references();
    test_make_constraint_interface_for_prvalues();
    test_make_constraint_interface_for_xvalues();
  }

  struct ANC : elastica::NonCopyable {
    template <typename S, typename T, typename I, typename... Args>
    void operator()(S& s, T, I, Args...) const {
      ++s;
    }
    mutable int times_called = {0};
  };

  void test_make_constraint_for_noncopyable_values() {
    auto a{ANC()};
    auto constraint_factory =
        make_constraint(std::move(a), [](auto, auto, auto, auto...) {});
    // now make a constrint
    auto my_constraint = constraint_factory();
    // constraint interface
    int mock_system = 2;
    int mock_time = 3;
    int mock_index = 1;
    my_constraint.constrain_values(mock_system, mock_time, mock_index);
    CHECK(mock_system == 3);
  }

  void test_make_constraint_for_noncopyable_rates() {
    auto a{ANC()};
    auto constraint_factory =
        make_constraint([](auto, auto, auto, auto...) {}, std::move(a));
    // now make a constrint
    auto my_constraint = constraint_factory();
    // constraint interface
    int mock_system = 2;
    int mock_time = 3;
    int mock_index = 1;
    my_constraint.constrain_rates(mock_system, mock_time, mock_index);
    CHECK(mock_system == 3);
  }

  void test_make_constraint_for_both_noncopyables() {
    auto a1{ANC()};
    auto a2{ANC()};
    auto constraint_factory = make_constraint(std::move(a1), std::move(a2));
    // now make a constrint
    auto my_constraint = constraint_factory();
    // constraint interface
    int mock_system = 2;
    int mock_time = 3;
    int mock_index = 1;
    my_constraint.constrain_values(mock_system, mock_time, mock_index);
    CHECK(mock_system == 3);
    my_constraint.constrain_rates(mock_system, mock_time, mock_index);
    CHECK(mock_system == 4);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Constraints.MakeConstraint.noncopyables",
                     "[Unit][Simulator][Constraints]") {
    // These always create a copy of the functors in question
    test_make_constraint_for_noncopyable_values();
    test_make_constraint_for_noncopyable_rates();
    test_make_constraint_for_both_noncopyables();
  }

  void test_make_constraint_throws_for_noncopyable_values() {
    auto a{ANC()};
    auto constraint_factory =
        make_constraint(std::move(a), [](auto, auto, auto, auto...) {});
    auto fc = constraint_factory();
    CHECK_THROWS_WITH(constraint_factory(),
                      Catch::Contains("the first functor is non-copyable"));
  }

  void test_make_constraint_throws_for_noncopyable_rates() {
    auto a{ANC()};
    auto constraint_factory =
        make_constraint([](auto, auto, auto, auto...) {}, std::move(a));
    auto fc = constraint_factory();
    CHECK_THROWS_WITH(constraint_factory(),
                      Catch::Contains("the second functor is non-copyable"));
  }

  void test_make_constraint_throws_for_both_noncopyables() {
    auto a1{ANC()};
    auto a2{ANC()};
    auto constraint_factory = make_constraint(std::move(a1), std::move(a2));
    auto fc = constraint_factory();
    CHECK_THROWS_WITH(constraint_factory(),
                      Catch::Contains("both the functors are non-copyable"));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.MakeConstraint.throws_for_repeated_use_of_noncopyables",
      "[Unit][Simulator][Constraints]") {
    // clang-format on
    // These always create a copy of the functors in question
    test_make_constraint_throws_for_noncopyable_values();
    test_make_constraint_throws_for_noncopyable_rates();
    test_make_constraint_throws_for_both_noncopyables();
  }

  struct CountedA : counter<CountedA> {
    template <typename S, typename T, typename I>
    void operator()(S&, T, I) const {}
  };

  struct CountedB : counter<CountedB> {
    explicit CountedB(int g) : times_called(g){};
    template <typename S, typename T, typename I>
    void operator()(S&, T, I) const {
      ++times_called;
    }
    mutable int times_called;
  };

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
    auto constraint_factory = make_constraint(a, b);

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
  }

}  // namespace
