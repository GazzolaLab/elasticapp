//
// Created by Tejaswin Parthasarathy on 1/17/21.
//

#include <array>
#include <deque>
#include <list>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Constraints/Constraint.hpp"
#include "Simulator/Constraints/Protocols.hpp"
#include "Utilities/NumericAt.hpp"
#include "Utilities/NumericRange.hpp"
#include "Utilities/ProtocolHelpers.hpp"

// we have more tests in Interface/Test_Constraint.cpp
// and additional tests in Constraints/Test_MakeConstraint.cpp
// we just give a examples and basic tests here

namespace elastica {

  namespace constraints {

    template <typename Registrars, typename ConcreteConstraint>
    struct TestConstraint
        : elastica::constraints::Constraint<Registrars, ConcreteConstraint> {
     public:
      using Parent =
          elastica::constraints::Constraint<Registrars, ConcreteConstraint>;

      // functions
      using Parent::Parent;
      using Parent::operator=;
      using Parent::constrain_primitives_internal;
      using Parent::constrain_rates_internal;

      // meta
      using Parent::derived_has_constrain_primitives;
      using Parent::derived_has_constrain_rates;
      using Parent::derived_has_no_constrain_primitives;
      using Parent::derived_has_no_constrain_rates;
    };

  }  // namespace constraints

}  // namespace elastica

#define ELASTICA_DEFINE_CONSTRAINT(class_name)                          \
  template <typename Registrars>                                        \
  class class_name;                                                     \
                                                                        \
  namespace Registrars {                                                \
    using class_name =                                                  \
        ::Registration::Registrar<::elastica::constraints::class_name>; \
  }                                                                     \
                                                                        \
  template <typename Registrars = tmpl::list<Registrars::class_name>>   \
  class class_name : public TestConstraint<Registrars, class_name<Registrars>>
//******************************************************************************

/// [cwam]
namespace elastica {
  namespace constraints {
    ELASTICA_DEFINE_CONSTRAINT(ConstraintExample) {
     public:
      explicit ConstraintExample(int& i) noexcept : i_(i){};
      ConstraintExample(ConstraintExample && other) noexcept : i_(other.i_) {}
      ~ConstraintExample() override = default;
      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /*time*/, System& /*system*/,
                                int /*index*/) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename System>
      void constrain_rates(TimeType /*time*/, System& /*system*/, int /*index*/)
          const {
        // do something
        --i_;
      }
      static std::string name() noexcept { return "ConstrainExample"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace constraints

}  // namespace elastica

/// [cwam]

/// [cwvo]

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithPrimitivesOnly) {
     public:
      explicit ConstraintWithPrimitivesOnly(int& f) noexcept : i_(f){};
      ConstraintWithPrimitivesOnly(ConstraintWithPrimitivesOnly &&
                                   other) noexcept
          : i_(other.i_) {}
      ~ConstraintWithPrimitivesOnly() override = default;
      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /*time*/, System& /*system*/,
                                int /*index*/) const {
        // do something
        ++i_;
      }
      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConstraintWithPrimitivesOnly";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace constraints

}  // namespace elastica

/// [cwvo]

/// [cwro]

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithRatesOnly) {
     public:
      explicit ConstraintWithRatesOnly(int& f) noexcept : i_(f){};
      ConstraintWithRatesOnly(ConstraintWithRatesOnly && other) noexcept
          : i_(other.i_) {}
      ~ConstraintWithRatesOnly() override = default;
      // no constrain primitives automatically assumed to be no-op
      template <typename TimeType, typename System>
      void constrain_rates(TimeType /*time*/, System& /*system*/, int /*index*/)
          const {
        // do something
        --i_;
      }
      static std::string name() noexcept { return "ConstraintWithRatesOnly"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace constraints

}  // namespace elastica

/// [cwro]

namespace {
  using namespace elastica::constraints;
  using namespace elastica;

  template <typename BC, typename... Args>
  constexpr auto check_and_make_constraint(Args&&... args) {
    static_assert(::tt::assert_conforms_to<BC, protocols::Constraint>,
                  "Constraint protocol violation!");
    return BC(std::forward<Args>(args)...);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Constraints.Constraint.with_all_members",
                     "[Unit][Simulator][Constraints]") {
    int i = 2;
    using BC = ::elastica::constraints::ConstraintExample<>;
    auto my_constraint(check_and_make_constraint<BC>(i));
    int mock_system = 2;
    int mock_time = 3;
    int mock_index = 1;

    // check-metaprogramming
    {
      static_assert(
          BC::derived_has_constrain_primitives<decltype(mock_time),
                                               decltype(mock_system)>(),
          "constraint metaprogramming failure!");
      static_assert(BC::derived_has_constrain_rates<decltype(mock_time),
                                                    decltype(mock_system)>(),
                    "constraint metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
      CHECK(my_constraint.i_ == 3);
      my_constraint.constrain_rates(mock_time, mock_system, mock_index);
      CHECK(my_constraint.i_ == 2);
    };

    // check at types
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_constraint.constrain_primitives_internal(mock_time, mock_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == (2 + 4));

      my_constraint.constrain_rates_internal(mock_time, mock_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == (2));
    };

    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_constraint.constrain_primitives_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == (2 + 4));

      my_constraint.constrain_rates_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == (2));
    };

    check_public_members();
    check_at();
    check_runtime_at();
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.Constraint.with_primitives_only",
      "[Unit][Simulator][Constraints]") {
    int i = 2;
    using BC = ::elastica::constraints::ConstraintWithPrimitivesOnly<>;
    auto my_constraint(check_and_make_constraint<BC>(i));
    int mock_system = 2;
    int mock_time = 3;
    int mock_index = 3;

    // check-metaprogramming
    {
      static_assert(
          BC::derived_has_constrain_primitives<decltype(mock_time),
                                               decltype(mock_system)>(),
          "constraint metaprogramming failure!");
      static_assert(BC::derived_has_no_constrain_rates<decltype(mock_time),
                                                       decltype(mock_system)>(),
                    "constraint metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      my_constraint.constrain_primitives(mock_time, mock_system, mock_index);
      CHECK(my_constraint.i_ == (2 + 1));
    };

    auto check_at = [&]() {
      // check at types
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_constraint.constrain_primitives_internal(mock_time, mock_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == (2 + 1 + 4));

      my_constraint.constrain_rates_internal(mock_time, mock_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == (2 + 1 + 4));
    };

    auto check_runtime_at = [&]() {
      // check at types
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_constraint.constrain_primitives_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == (2 + 1 + 4 + 4));

      my_constraint.constrain_rates_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == (2 + 1 + 4 + 4));
    };

    check_public_members();
    check_at();
    check_runtime_at();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Constraints.Constraint.with_rates_only",
                     "[Unit][Simulator][Constraints]") {
    int i = 10;
    using BC = ::elastica::constraints::ConstraintWithRatesOnly<>;
    auto my_constraint(check_and_make_constraint<BC>(i));
    int mock_system = 2;
    int mock_time = 3;
    int mock_index = 3;

    // check-metaprogramming
    {
      static_assert(
          BC::derived_has_no_constrain_primitives<decltype(mock_time),
                                                  decltype(mock_system)>(),
          "constraint metaprogramming failure!");
      static_assert(BC::derived_has_constrain_rates<decltype(mock_time),
                                                    decltype(mock_system)>(),
                    "constraint metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      // check the public memebr
      my_constraint.constrain_rates(mock_time, mock_system, mock_index);
      CHECK(my_constraint.i_ == (10 - 1));
    };

    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_constraint.constrain_primitives_internal(mock_time, mock_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == (10 - 1));
      my_constraint.constrain_rates_internal(mock_time, mock_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == (10 - 1 - 4));
    };

    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_constraint.constrain_primitives_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == (10 - 1 - 4));
      my_constraint.constrain_rates_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == (10 - 1 - 4 - 4));
    };

    check_public_members();
    check_at();
    check_runtime_at();
  }

}  // namespace

/// [cwsv]
namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithPrimitivesForSpecificSystem) {
     public:
      explicit ConstraintWithPrimitivesForSpecificSystem(int& i) noexcept
          : i_(i){};
      ConstraintWithPrimitivesForSpecificSystem(
          ConstraintWithPrimitivesForSpecificSystem && other) noexcept
          : i_(other.i_) {}
      ~ConstraintWithPrimitivesForSpecificSystem() override = default;
      // overloaded for specialized systems
      template <typename TimeType>
      void constrain_primitives(TimeType /*time*/, int& /*system*/,
                                int /*index*/) const {
        // do something
        ++i_;
      }
      template <typename TimeType>
      void constrain_primitives(TimeType /*time*/, float& /*system*/,
                                int /*index*/) const {
        // do something
        --i_;
      }
      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConstraintWithPrimitivesForSpecificSystem";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace constraints

}  // namespace elastica
/// [cwsv]

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.Constraint.primitives_for_specific_system",
      "[Unit][Simulator][Constraints]") {
    int i = 2;
    using BC =
        ::elastica::constraints::ConstraintWithPrimitivesForSpecificSystem<>;
    auto my_constraint(check_and_make_constraint<BC>(i));
    int mock_time = 3;
    int mock_index = 3;

    int mock_int_system = 2;
    float mock_float_system = 2.0F;

    // check-metaprogramming
    {
      static_assert(
          BC::derived_has_constrain_primitives<decltype(mock_time),
                                               decltype(mock_int_system)>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_constrain_primitives<decltype(mock_time),
                                               decltype(mock_float_system)>(),
          "constraint metaprogramming failure!");
      static_assert(BC::derived_has_no_constrain_primitives<decltype(mock_time),
                                                            double>(),
                    "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_no_constrain_primitives<decltype(mock_time), char>(),
          "constraint metaprogramming failure!");

      static_assert(
          BC::derived_has_no_constrain_rates<decltype(mock_time),
                                             decltype(mock_int_system)>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_no_constrain_rates<decltype(mock_time),
                                             decltype(mock_float_system)>(),
          "constraint metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_constraint.constrain_primitives(mock_time, mock_int_system,
                                         mock_index);
      my_constraint.constrain_primitives(mock_time, mock_int_system,
                                         mock_index);
      const int expected1 = (2 + 2);
      CHECK(my_constraint.i_ == expected1);

      my_constraint.constrain_primitives(mock_time, mock_float_system,
                                         mock_index);
      const int expected2 = expected1 - 1;
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain primitives for the above cases
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_constraint.i_;
      my_constraint.constrain_primitives_internal(mock_time, mock_int_system,
                                                  mock_indices);
      const int expected3 = expected2 + 4;
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_rates_internal(mock_time, mock_int_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_rates_internal(mock_time, mock_float_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected3);

      // check constrain primitives for the above cases
      my_constraint.constrain_primitives_internal(mock_time, mock_float_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain primitives for the above cases
    auto check_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_constraint.i_;
      double mock_double_system(5.0);
      my_constraint.constrain_primitives_internal(mock_time, mock_double_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected);

      char mock_char('a');
      my_constraint.constrain_primitives_internal(mock_time, mock_char,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected);
    };

    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_constraint.i_;
      my_constraint.constrain_primitives_internal(
          mock_time, mock_int_system, mock_indices.data(), mock_indices.size());
      const int expected3 = expected2 + 4;
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_rates_internal(
          mock_time, mock_int_system, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_rates_internal(mock_time, mock_float_system,
                                             mock_indices.data(),
                                             mock_indices.size());
      CHECK(my_constraint.i_ == expected3);

      // check constrain primitives for the above cases
      my_constraint.constrain_primitives_internal(mock_time, mock_float_system,
                                                  mock_indices.data(),
                                                  mock_indices.size());
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain primitives for the above cases
    auto check_runtime_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_constraint.i_;
      double mock_double_system(5.0);
      my_constraint.constrain_primitives_internal(mock_time, mock_double_system,
                                                  mock_indices.data(),
                                                  mock_indices.size());
      CHECK(my_constraint.i_ == expected);

      char mock_char('a');
      my_constraint.constrain_primitives_internal(
          mock_time, mock_char, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == expected);
    };

    check_public_members();
    check_at();
    check_at_is_noop_for_nonoverloaded_types();
    check_runtime_at();
    check_runtime_at_is_noop_for_nonoverloaded_types();
  }
}  // namespace

/// [cwtv]
namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithPrimitivesForTemplatedSystems) {
     public:
      explicit ConstraintWithPrimitivesForTemplatedSystems(int& i) noexcept
          : i_(i){};
      ConstraintWithPrimitivesForTemplatedSystems(
          ConstraintWithPrimitivesForTemplatedSystems && other) noexcept
          : i_(other.i_) {}
      ~ConstraintWithPrimitivesForTemplatedSystems() override = default;
      // overloaded for specialized systems

      // in the base class we have <Time,System>
      // can we do partial specialization?
      template <typename TimeType, typename T>
      void constrain_primitives(TimeType /*time*/, std::vector<T>& /*system*/,
                                int /*index*/) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename T>
      void constrain_primitives(TimeType /*time*/, std::list<T>& /*system*/,
                                int /*index*/) const {
        // do something
        --i_;
      }

      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConstraintWithPrimitivesForTemplatedSystems";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace constraints

}  // namespace elastica
/// [cwtv]

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.Constraint.primitives_for_templated_system",
      "[Unit][Simulator][Constraints]") {
    int i = 2;
    using BC =
        ::elastica::constraints::ConstraintWithPrimitivesForTemplatedSystems<>;
    auto my_constraint(check_and_make_constraint<BC>(i));
    int mock_time = 3;
    int mock_index = 3;

    std::vector<int> mock_vector_system{1, 2, 3};
    std::list<int> mock_list_system = {4, 5, 6};

    // check-metaprogramming
    {
      static_assert(
          BC::derived_has_constrain_primitives<decltype(mock_time),
                                               decltype(mock_vector_system)>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_constrain_primitives<decltype(mock_time),
                                               decltype(mock_list_system)>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_no_constrain_primitives<decltype(mock_time),
                                                  std::array<int, 2>>(),
          "constraint metaprogramming failure!");
      static_assert(BC::derived_has_no_constrain_primitives<decltype(mock_time),
                                                            std::deque<int>>(),
                    "constraint metaprogramming failure!");

      static_assert(
          BC::derived_has_no_constrain_rates<decltype(mock_time),
                                             decltype(mock_vector_system)>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_no_constrain_rates<decltype(mock_time),
                                             decltype(mock_list_system)>(),
          "constraint metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_constraint.constrain_primitives(mock_time, mock_vector_system,
                                         mock_index);
      my_constraint.constrain_primitives(mock_time, mock_vector_system,
                                         mock_index);
      const int expected1 = (2 + 2);
      CHECK(my_constraint.i_ == expected1);

      my_constraint.constrain_primitives(mock_time, mock_list_system,
                                         mock_index);
      const int expected2 = expected1 - 1;
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain primitives for the above cases
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_constraint.i_;
      my_constraint.constrain_primitives_internal(mock_time, mock_vector_system,
                                                  mock_indices);
      const int expected3 = expected2 + 4;
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_rates_internal(mock_time, mock_vector_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_rates_internal(mock_time, mock_list_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected3);

      // check constrain primitives for the above cases
      my_constraint.constrain_primitives_internal(mock_time, mock_list_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected2);
    };

    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_constraint.i_;
      my_constraint.constrain_primitives_internal(mock_time, mock_vector_system,
                                                  mock_indices.data(),
                                                  mock_indices.size());
      const int expected3 = expected2 + 4;
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_rates_internal(mock_time, mock_vector_system,
                                             mock_indices.data(),
                                             mock_indices.size());
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_rates_internal(mock_time, mock_list_system,
                                             mock_indices.data(),
                                             mock_indices.size());
      CHECK(my_constraint.i_ == expected3);

      // check constrain primitives for the above cases
      my_constraint.constrain_primitives_internal(mock_time, mock_list_system,
                                                  mock_indices.data(),
                                                  mock_indices.size());
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain primitives for the above cases
    auto check_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_constraint.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_constraint.constrain_primitives_internal(mock_time, mock_array_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_constraint.constrain_primitives_internal(mock_time, mock_deque,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected);
    };

    // check constrain primitives for the above cases
    auto check_runtime_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_constraint.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_constraint.constrain_primitives_internal(mock_time, mock_array_system,
                                                  mock_indices.data(),
                                                  mock_indices.size());
      CHECK(my_constraint.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_constraint.constrain_primitives_internal(
          mock_time, mock_deque, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == expected);
    };

    check_public_members();
    check_at();
    check_at_is_noop_for_nonoverloaded_types();
    check_runtime_at();
    check_runtime_at_is_noop_for_nonoverloaded_types();
  }
}  // namespace

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithRatesForSpecificSystem) {
     public:
      explicit ConstraintWithRatesForSpecificSystem(int& i) noexcept : i_(i){};
      ConstraintWithRatesForSpecificSystem(
          ConstraintWithRatesForSpecificSystem && other) noexcept
          : i_(other.i_) {}
      ~ConstraintWithRatesForSpecificSystem() override = default;
      // overloaded for specialized systems
      template <typename TimeType>
      void constrain_rates(TimeType /*time*/, int& /*system*/, int /*index*/)
          const {
        // do something
        ++i_;
      }
      template <typename TimeType>
      void constrain_rates(TimeType /*time*/, float& /*system*/, int /*index*/)
          const {
        // do something
        --i_;
      }
      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConstraintWithRatesForSpecificSystem";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace constraints

}  // namespace elastica

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.Constraint.with_rates_for_specific_system",
      "[Unit][Simulator][Constraints]") {
    int i = 2;
    using BC = ::elastica::constraints::ConstraintWithRatesForSpecificSystem<>;
    auto my_constraint(check_and_make_constraint<BC>(i));
    int mock_time = 3;
    int mock_index = 3;

    int mock_int_system = 2;
    float mock_float_system = 2.0F;

    // check-metaprogramming
    {
      static_assert(
          BC::derived_has_constrain_rates<decltype(mock_time),
                                          decltype(mock_int_system)>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_constrain_rates<decltype(mock_time),
                                          decltype(mock_float_system)>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_no_constrain_rates<decltype(mock_time), double>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_no_constrain_rates<decltype(mock_time), char>(),
          "constraint metaprogramming failure!");

      static_assert(
          BC::derived_has_no_constrain_primitives<decltype(mock_time),
                                                  decltype(mock_int_system)>(),
          "constraint metaprogramming failure!");
      static_assert(BC::derived_has_no_constrain_primitives<
                        decltype(mock_time), decltype(mock_float_system)>(),
                    "constraint metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_constraint.constrain_rates(mock_time, mock_int_system, mock_index);
      my_constraint.constrain_rates(mock_time, mock_int_system, mock_index);
      const int expected1 = (2 + 2);
      CHECK(my_constraint.i_ == expected1);

      my_constraint.constrain_rates(mock_time, mock_float_system, mock_index);
      const int expected2 = expected1 - 1;
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain rates for the above cases
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_constraint.i_;
      my_constraint.constrain_rates_internal(mock_time, mock_int_system,
                                             mock_indices);
      const int expected3 = expected2 + 4;
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_primitives_internal(mock_time, mock_int_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_primitives_internal(mock_time, mock_float_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected3);

      // check constrain rates for the above cases
      my_constraint.constrain_rates_internal(mock_time, mock_float_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain rates for the above cases
    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_constraint.i_;
      my_constraint.constrain_rates_internal(
          mock_time, mock_int_system, mock_indices.data(), mock_indices.size());
      const int expected3 = expected2 + 4;
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_primitives_internal(
          mock_time, mock_int_system, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_primitives_internal(mock_time, mock_float_system,
                                                  mock_indices.data(),
                                                  mock_indices.size());
      CHECK(my_constraint.i_ == expected3);

      // check constrain rates for the above cases
      my_constraint.constrain_rates_internal(mock_time, mock_float_system,
                                             mock_indices.data(),
                                             mock_indices.size());
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain rates for the above cases
    auto check_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_constraint.i_;
      double mock_double_system(5.0);
      my_constraint.constrain_rates_internal(mock_time, mock_double_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected);

      char mock_char('a');
      my_constraint.constrain_rates_internal(mock_time, mock_char,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected);
    };

    // check constrain rates for the above cases
    auto check_runtime_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_constraint.i_;
      double mock_double_system(5.0);
      my_constraint.constrain_rates_internal(mock_time, mock_double_system,
                                             mock_indices.data(),
                                             mock_indices.size());
      CHECK(my_constraint.i_ == expected);

      char mock_char('a');
      my_constraint.constrain_rates_internal(
          mock_time, mock_char, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == expected);
    };

    check_public_members();
    check_at();
    check_at_is_noop_for_nonoverloaded_types();
    check_runtime_at();
    check_runtime_at_is_noop_for_nonoverloaded_types();
  }
}  // namespace

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithRatesForTemplatedSystems) {
     public:
      explicit ConstraintWithRatesForTemplatedSystems(int& i) noexcept
          : i_(i){};
      ConstraintWithRatesForTemplatedSystems(
          ConstraintWithRatesForTemplatedSystems && other) noexcept
          : i_(other.i_) {}
      ~ConstraintWithRatesForTemplatedSystems() override = default;
      // overloaded for specialized systems

      // in the base class we have <Time,System>
      // can we do partial specialization?
      template <typename TimeType, typename T>
      void constrain_rates(TimeType /*time*/, std::vector<T>& /*system*/,
                           int /*index*/) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename T>
      void constrain_rates(TimeType /*time*/, std::list<T>& /*system*/,
                           int /*index*/) const {
        // do something
        --i_;
      }

      // no constrain rates, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ConstraintWithRatesForTemplatedSystems";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace constraints

}  // namespace elastica

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Constraints.Constraint.with_rates_for_templated_system",
      "[Unit][Simulator][Constraints]") {
    int i = 2;
    using BC =
        ::elastica::constraints::ConstraintWithRatesForTemplatedSystems<>;
    auto my_constraint(check_and_make_constraint<BC>(i));
    int mock_time = 3;
    int mock_index = 3;

    std::vector<int> mock_vector_system{1, 2, 3};
    std::list<int> mock_list_system = {4, 5, 6};

    // check-metaprogramming
    {
      static_assert(
          BC::derived_has_constrain_rates<decltype(mock_time),
                                          decltype(mock_vector_system)>(),
          "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_constrain_rates<decltype(mock_time),
                                          decltype(mock_list_system)>(),
          "constraint metaprogramming failure!");
      static_assert(BC::derived_has_no_constrain_rates<decltype(mock_time),
                                                       std::array<int, 2>>(),
                    "constraint metaprogramming failure!");
      static_assert(BC::derived_has_no_constrain_rates<decltype(mock_time),
                                                       std::deque<int>>(),
                    "constraint metaprogramming failure!");

      static_assert(BC::derived_has_no_constrain_primitives<
                        decltype(mock_time), decltype(mock_vector_system)>(),
                    "constraint metaprogramming failure!");
      static_assert(
          BC::derived_has_no_constrain_primitives<decltype(mock_time),
                                                  decltype(mock_list_system)>(),
          "constraint metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_constraint.constrain_rates(mock_time, mock_vector_system, mock_index);
      my_constraint.constrain_rates(mock_time, mock_vector_system, mock_index);
      const int expected1 = (2 + 2);
      CHECK(my_constraint.i_ == expected1);

      my_constraint.constrain_rates(mock_time, mock_list_system, mock_index);
      const int expected2 = expected1 - 1;
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain rates for the above cases
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_constraint.i_;
      my_constraint.constrain_rates_internal(mock_time, mock_vector_system,
                                             mock_indices);
      const int expected3 = expected2 + 4;
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_primitives_internal(mock_time, mock_vector_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_primitives_internal(mock_time, mock_list_system,
                                                  mock_indices);
      CHECK(my_constraint.i_ == expected3);

      // check constrain rates for the above cases
      my_constraint.constrain_rates_internal(mock_time, mock_list_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain rates for the above cases
    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_constraint.i_;
      my_constraint.constrain_rates_internal(mock_time, mock_vector_system,
                                             mock_indices.data(),
                                             mock_indices.size());
      const int expected3 = expected2 + 4;
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_primitives_internal(mock_time, mock_vector_system,
                                                  mock_indices.data(),
                                                  mock_indices.size());
      CHECK(my_constraint.i_ == expected3);

      // noop
      my_constraint.constrain_primitives_internal(mock_time, mock_list_system,
                                                  mock_indices.data(),
                                                  mock_indices.size());
      CHECK(my_constraint.i_ == expected3);

      // check constrain rates for the above cases
      my_constraint.constrain_rates_internal(mock_time, mock_list_system,
                                             mock_indices.data(),
                                             mock_indices.size());
      CHECK(my_constraint.i_ == expected2);
    };

    // check constrain rates for the above cases
    auto check_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_constraint.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_constraint.constrain_rates_internal(mock_time, mock_array_system,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_constraint.constrain_rates_internal(mock_time, mock_deque,
                                             mock_indices);
      CHECK(my_constraint.i_ == expected);
    };

    // check constrain rates for the above cases
    auto check_runtime_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_constraint.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_constraint.constrain_rates_internal(mock_time, mock_array_system,
                                             mock_indices.data(),
                                             mock_indices.size());
      CHECK(my_constraint.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_constraint.constrain_rates_internal(
          mock_time, mock_deque, mock_indices.data(), mock_indices.size());
      CHECK(my_constraint.i_ == expected);
    };

    check_public_members();
    check_at();
    check_at_is_noop_for_nonoverloaded_types();
    check_runtime_at();
    check_runtime_at_is_noop_for_nonoverloaded_types();
  }
}  // namespace
