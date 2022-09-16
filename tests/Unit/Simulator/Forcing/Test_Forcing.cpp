//
// Created by Tejaswin Parthasarathy on 1/17/21.
//
#include <array>
#include <deque>
#include <list>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Forcing/Forcing.hpp"
#include "Simulator/Forcing/Protocols.hpp"
#include "Utilities/NumericAt.hpp"
#include "Utilities/NumericRange.hpp"
#include "Utilities/ProtocolHelpers.hpp"

// we have more tests in Interface/Test_Forcing.cpp
// and additional tests in Forcing/Test_MakeForcing.cpp
// we just give a examples and basic tests here

namespace elastica {

  namespace forcing {

    template <typename Registrars, typename ConcreteForcing>
    struct TestForcing
        : elastica::forcing::Forcing<Registrars, ConcreteForcing> {
     public:
      using Parent = elastica::forcing::Forcing<Registrars, ConcreteForcing>;

      // functions
      using Parent::Parent;
      using Parent::operator=;
      using Parent::apply_forces_internal;
      using Parent::apply_torques_internal;

      // meta
      using Parent::derived_has_apply_forces;
      using Parent::derived_has_apply_forces_on_range;
      using Parent::derived_has_apply_torques;
      using Parent::derived_has_apply_torques_on_range;
      using Parent::derived_has_no_apply_forces;
      using Parent::derived_has_no_apply_forces_on_range;
      using Parent::derived_has_no_apply_torques;
      using Parent::derived_has_no_apply_torques_on_range;
    };

  }  // namespace forcing

}  // namespace elastica

#define ELASTICA_DEFINE_FORCING(class_name)                           \
  template <typename Registrars>                                      \
  class class_name;                                                   \
                                                                      \
  namespace Registrars {                                              \
    using class_name =                                                \
        ::Registration::Registrar<::elastica::forcing::class_name>;   \
  }                                                                   \
                                                                      \
  template <typename Registrars = tmpl::list<Registrars::class_name>> \
  class class_name : public TestForcing<Registrars, class_name<Registrars>>
//******************************************************************************

/// [fwam]
namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingExample) {
     public:
      explicit ForcingExample(int& i) noexcept : i_(i){};
      ForcingExample(ForcingExample && other) noexcept : i_(other.i_) {}
      ~ForcingExample() override = default;
      template <typename TimeType, typename System>
      void apply_forces(TimeType /*time*/, System& /*system*/,
                        std::size_t /* index */) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename System>
      void apply_torques(TimeType /*time*/, System& /*system*/,
                         std::size_t /* index */) const {
        // do something
        --i_;
      }
      static std::string name() noexcept { return "ForcingExample"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

/// [fwam]

/// [fwfo]

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithForcesOnly) {
     public:
      explicit ForcingWithForcesOnly(int& f) noexcept : i_(f){};
      ForcingWithForcesOnly(ForcingWithForcesOnly && other) noexcept
          : i_(other.i_) {}
      ~ForcingWithForcesOnly() override = default;
      template <typename TimeType, typename System>
      void apply_forces(TimeType /*time*/, System& /*system*/,
                        std::size_t /* index */) const {
        // do something
        ++i_;
      }
      // no apply torques, automatically assumed to be no-op
      static std::string name() noexcept { return "ForcingWithForcesOnly"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

/// [fwfo]

/// [fwto]

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithTorquesOnly) {
     public:
      explicit ForcingWithTorquesOnly(int& f) noexcept : i_(f){};
      ForcingWithTorquesOnly(ForcingWithTorquesOnly && other) noexcept
          : i_(other.i_) {}
      ~ForcingWithTorquesOnly() override = default;
      // no apply forces automatically assumed to be no-op
      template <typename TimeType, typename System>
      void apply_torques(TimeType /*time*/, System& /*system*/,
                         std::size_t /* index */) const {
        // do something
        --i_;
      }
      static std::string name() noexcept { return "ForcingWithTorquesOnly"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

/// [fwto]

namespace {
  using namespace elastica::forcing;
  using namespace elastica;

  template <typename F, typename... Args>
  constexpr auto check_and_make_forcing(Args&&... args) {
    static_assert(::tt::assert_conforms_to<F, protocols::Forcing>,
                  "Forcing protocol violation!");
    return F(std::forward<Args>(args)...);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.Forcing.with_all_members",
                     "[Unit][Simulator][Forcing]") {
    int i = 2;
    using F = ::elastica::forcing::ForcingExample<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_system = 2;
    int mock_time = 3;
    std::size_t mock_index = 1;

    // check-metaprogramming
    {
      static_assert(F::derived_has_apply_forces<decltype(mock_time),
                                                decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_apply_torques<decltype(mock_time),
                                                 decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces_on_range<decltype(mock_time),
                                                  decltype(mock_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time),
                                                   decltype(mock_system)>(),
          "forcing metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      const int expected = my_forcing.i_;
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      CHECK(my_forcing.i_ == expected + 1);
      my_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(my_forcing.i_ == expected);
    };

    // check at types
    auto check_at = [&]() {
      const int expected = my_forcing.i_;
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_indices);
      const int expected2 = expected + 4;
      CHECK(my_forcing.i_ == expected2);

      my_forcing.apply_torques_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == expected);
    };

    // check at types
    auto check_runtime_at = [&]() {
      const int expected = my_forcing.i_;
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_forces_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      const int expected2 = expected + 4;
      CHECK(my_forcing.i_ == expected2);

      my_forcing.apply_torques_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == expected);
    };

    auto check_range = [&]() {
      const int expected = my_forcing.i_;
      auto mock_range = numeric_range(1UL, 10UL, 1);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_range);
      const int expected2 = expected + (10 - 1);
      CHECK(my_forcing.i_ == (expected2));
      my_forcing.apply_torques_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (expected));
    };

    check_public_members();
    check_at();
    check_runtime_at();
    check_range();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.Forcing.with_forces_only",
                     "[Unit][Simulator][Forcing]") {
    int i = 2;
    using F = ::elastica::forcing::ForcingWithForcesOnly<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_system = 2;
    int mock_time = 3;
    std::size_t mock_index = 3;

    // check-metaprogramming
    {
      static_assert(F::derived_has_apply_forces<decltype(mock_time),
                                                decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_torques<decltype(mock_time),
                                                    decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces_on_range<decltype(mock_time),
                                                  decltype(mock_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time),
                                                   decltype(mock_system)>(),
          "forcing metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      const int expected = my_forcing.i_;
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      CHECK(my_forcing.i_ == expected + 1);
    };

    auto check_at = [&]() {
      // check at types
      const int expected = my_forcing.i_;
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == (expected + 4));

      my_forcing.apply_torques_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == (expected + 4));
    };

    auto check_runtime_at = [&]() {
      // check at types
      const int expected = my_forcing.i_;
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_forces_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == (expected + 4));

      my_forcing.apply_torques_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == (expected + 4));
    };

    auto check_range = [&]() {
      // check range types
      const int expected = my_forcing.i_;
      auto mock_range = numeric_range(1UL, 10UL, 1);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (expected + (10 - 1)));
      my_forcing.apply_torques_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (expected + (10 - 1)));
    };

    check_public_members();
    check_at();
    check_runtime_at();
    check_range();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.Forcing.with_torques_only",
                     "[Unit][Simulator][Forcing]") {
    int i = 10;
    using F = ::elastica::forcing::ForcingWithTorquesOnly<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_system = 2;
    int mock_time = 3;
    std::size_t mock_index = 3;

    // check-metaprogramming
    {
      static_assert(F::derived_has_no_apply_forces<decltype(mock_time),
                                                   decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_apply_torques<decltype(mock_time),
                                                 decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces_on_range<decltype(mock_time),
                                                  decltype(mock_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time),
                                                   decltype(mock_system)>(),
          "forcing metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      // check the public memebr
      const int expected = my_forcing.i_;
      my_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(my_forcing.i_ == (expected - 1));
    };

    auto check_at = [&]() {
      // check at types
      const int expected = my_forcing.i_;
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == (expected));
      my_forcing.apply_torques_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == (expected - 4));
    };

    auto check_runtime_at = [&]() {
      // check at types
      const int expected = my_forcing.i_;
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_forces_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == (expected));
      my_forcing.apply_torques_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == (expected - 4));
    };

    auto check_range = [&]() {
      // check range types
      const int expected = my_forcing.i_;
      auto mock_range = numeric_range(1UL, 10UL, 1);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (expected));
      my_forcing.apply_torques_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (expected - (10 - 1)));
    };

    check_public_members();
    check_at();
    check_runtime_at();
    check_range();
  }

}  // namespace

/// [fwfor]

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithForcesInRange) {
     public:
      explicit ForcingWithForcesInRange(int& f) noexcept : i_(f){};
      ForcingWithForcesInRange(ForcingWithForcesInRange && other) noexcept
          : i_(other.i_) {}
      ~ForcingWithForcesInRange() override = default;
      template <typename TimeType, typename System>
      void apply_forces(TimeType /*time*/, System& /*system*/,
                        std::size_t /* index */) const {
        // do nothing
      }
      template <typename TimeType, typename System>
      void apply_forces_on_range(TimeType /*time*/, System& /*system*/,
                                 int /*start*/, int /*stop*/) const {
        i_ += 10;
      }
      // no apply torques, automatically assumed to be no-op
      static std::string name() noexcept { return "ForcingWithForcesInRange"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

/// [fwfor]

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.Forcing.with_ranged_forces",
                     "[Unit][Simulator][Forcing]") {
    int i = 2;

    using F = ::elastica::forcing::ForcingWithForcesInRange<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_system = 2;
    int mock_time = 3;
    std::size_t mock_index = 3;

    // check-metaprogramming
    {
      static_assert(F::derived_has_apply_forces<decltype(mock_time),
                                                decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_torques<decltype(mock_time),
                                                    decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_apply_forces_on_range<decltype(mock_time),
                                               decltype(mock_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time),
                                                   decltype(mock_system)>(),
          "forcing metaprogramming failure!");
    }

    auto check_public_members = [&]() {
      // check the public member
      const int expected = my_forcing.i_;
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      CHECK(my_forcing.i_ == (expected));
    };

    auto check_range = [&]() {
      // compact indices still do nothing
      const int expected = my_forcing.i_;
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == (expected));

      // compact indices with runtime is invariant
      my_forcing.apply_forces_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == (expected));

      // ranged indices with a step does nothing too
      auto mock_range_with_step = numeric_range(1UL, 10UL, 2);
      my_forcing.apply_forces_internal(mock_time, mock_system,
                                       mock_range_with_step);
      CHECK(my_forcing.i_ == (expected));

      // ranged indices with a step of 1 does what we want it to do
      auto mock_range = numeric_range(1UL, 10UL, 1);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (expected + 10));
    };

    check_public_members();
    check_range();
  }
}  // namespace

/// [fwtor]

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithTorquesInRange) {
     public:
      explicit ForcingWithTorquesInRange(int& f) noexcept : i_(f){};
      ForcingWithTorquesInRange(ForcingWithTorquesInRange && other) noexcept
          : i_(other.i_) {}
      ~ForcingWithTorquesInRange() override = default;
      template <typename TimeType, typename System>
      void apply_torques(TimeType /*time*/, System& /*system*/,
                         std::size_t /* index */) const {
        // do nothing
      }
      template <typename TimeType, typename System>
      void apply_torques_on_range(TimeType /*time*/, System& /*system*/,
                                  int /*start*/, int /*stop*/) const {
        i_ -= 10;
      }
      // no apply torques, automatically assumed to be no-op
      static std::string name() noexcept { return "ForcingWithTorquesInRange"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

/// [fwtor]

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.Forcing.with_ranged_torques",
                     "[Unit][Simulator][Forcing]") {
    int i = 2;
    using F = ::elastica::forcing::ForcingWithTorquesInRange<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_system = 2;
    int mock_time = 3;
    std::size_t mock_index = 3;

    // check-metaprogramming
    {
      static_assert(F::derived_has_no_apply_forces<decltype(mock_time),
                                                   decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_apply_torques<decltype(mock_time),
                                                 decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces_on_range<decltype(mock_time),
                                                  decltype(mock_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_apply_torques_on_range<decltype(mock_time),
                                                decltype(mock_system)>(),
          "forcing metaprogramming failure!");
    }

    auto check_public_members = [&]() {  // check the public member
      const int expected = my_forcing.i_;
      my_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(my_forcing.i_ == (expected));
    };

    auto check_range = [&]() {
      const int expected = my_forcing.i_;

      // compact indices still do nothing
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_torques_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == (expected));

      my_forcing.apply_torques_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == (expected));

      // ranged indices with a step does nothing too
      auto mock_range_with_step = numeric_range(1UL, 10UL, 2);
      my_forcing.apply_torques_internal(mock_time, mock_system,
                                        mock_range_with_step);
      CHECK(my_forcing.i_ == (expected));

      // ranged indices with a step of 1 does what we want it to do
      auto mock_range = numeric_range(1UL, 10UL, 1);
      my_forcing.apply_torques_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (expected - (10)));
    };

    check_public_members();
    check_range();
  }
}  // namespace

/// [fwamr]
namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(RangedForcingExample) {
     public:
      explicit RangedForcingExample(int& i) noexcept : i_(i){};
      RangedForcingExample(RangedForcingExample && other) noexcept
          : i_(other.i_) {}
      ~RangedForcingExample() override = default;
      template <typename TimeType, typename System>
      void apply_forces(TimeType /*time*/, System& /*system*/,
                        std::size_t /* index */) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename System>
      void apply_forces_on_range(TimeType /*time*/, System& /*system*/,
                                 int /*start*/, int /*stop*/) const {
        // do something
        i_ += 2;
      }
      template <typename TimeType, typename System>
      void apply_torques(TimeType /*time*/, System& /*system*/,
                         std::size_t /* index */) const {
        // do something
        --i_;
      }
      template <typename TimeType, typename System>
      void apply_torques_on_range(TimeType /*time*/, System& /*system*/,
                                  int /*start*/, int /*stop*/) const {
        i_ -= 2;
      }

      static std::string name() noexcept { return "RangedForcingExample"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

/// [fwamr]

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.Forcing.with_all_ranged",
                     "[Unit][Simulator][Forcing]") {
    int i = 2;
    using F = ::elastica::forcing::RangedForcingExample<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_system = 2;
    int mock_time = 3;
    std::size_t mock_index = 3;

    // check-metaprogramming
    {
      static_assert(F::derived_has_apply_forces<decltype(mock_time),
                                                decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_apply_torques<decltype(mock_time),
                                                 decltype(mock_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_apply_forces_on_range<decltype(mock_time),
                                               decltype(mock_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_apply_torques_on_range<decltype(mock_time),
                                                decltype(mock_system)>(),
          "forcing metaprogramming failure!");
    }

    auto check_public_members = [&]() {  // check the public member
      const int expected = my_forcing.i_;

      // check the public member
      my_forcing.apply_forces(mock_time, mock_system, mock_index);
      CHECK(my_forcing.i_ == (expected + 1));
      my_forcing.apply_torques(mock_time, mock_system, mock_index);
      CHECK(my_forcing.i_ == (expected));
    };

    auto check_range = [&]() {
      const int expected = my_forcing.i_;

      // compact indices
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == (expected + 4));
      my_forcing.apply_torques_internal(mock_time, mock_system, mock_indices);
      CHECK(my_forcing.i_ == (expected));

      // compact indices at runtime
      my_forcing.apply_forces_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == (expected + 4));
      my_forcing.apply_torques_internal(
          mock_time, mock_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == (expected));

      // ranged indices with a step
      auto mock_range_with_step = numeric_range(1UL, 10UL, 2);
      my_forcing.apply_forces_internal(mock_time, mock_system,
                                       mock_range_with_step);
      CHECK(my_forcing.i_ == (expected + 5));
      my_forcing.apply_torques_internal(mock_time, mock_system,
                                        mock_range_with_step);
      CHECK(my_forcing.i_ == (expected));

      // ranged indices with a step of 1
      auto mock_range = numeric_range(1UL, 10UL, 1);
      my_forcing.apply_forces_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (expected + (2)));
      my_forcing.apply_torques_internal(mock_time, mock_system, mock_range);
      CHECK(my_forcing.i_ == (2));
    };

    check_public_members();
    check_range();
  }

}  // namespace

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(IncorrectForcingWithRange) {
     public:
      explicit IncorrectForcingWithRange(int& f) noexcept : i_(f){};
      IncorrectForcingWithRange(IncorrectForcingWithRange && other) noexcept
          : i_(other.i_) {}
      ~IncorrectForcingWithRange() override = default;
      /* Oops, should have provided this members!
      template <typename TimeType, typename System>
      void apply_torques(TimeType time, System& system, int index) const {
        // do nothing
      }

      template <typename TimeType, typename System>
      void apply_forces(TimeType time, System& system, int index) const {
        // do nothing
      }
      */
      template <typename TimeType, typename System>
      void apply_forces_on_range(TimeType /*time*/, System& /*system*/,
                                 int /*start*/, int /*stop*/) const {
        // do something
        i_ += 10;
      }

      template <typename TimeType, typename System>
      void apply_torques_on_range(TimeType /*time*/, System& /*system*/,
                                  int /*start*/, int /*stop*/) const {
        i_ -= 10;
      }
      // no apply torques, automatically assumed to be no-op
      static std::string name() noexcept { return "IncorrectForcingWithRange"; }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

// namespace {
//  // This should always fail!
//
//  ELASTICA_BOTCHED_TEST_CASE(
//      "Unit.Simulator.Forcing.Forcing.with_incorrect_ranged_torques",
//      "[Unit][Simulator][Forcing]") {
//    int i = 2;
//    ::elastica::forcing::IncorrectForcingWithRange<> my_forcing(i);
//    int mock_system = 2;
//    int mock_time = 3;
//    std::size_t mock_index = 3;
//    // ranged indices with a step of 1 does what we want it to do
//    auto mock_range = numeric_range(1UL, 10UL, 1);
//    my_forcing.apply_forces_internal(mock_time, mock_system, mock_range);
//    CHECK(my_forcing.i_ == (2 - (10)));
//  }
//
//}  // namespace

/// [fwsf]
namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithForcesForSpecificSystem) {
     public:
      explicit ForcingWithForcesForSpecificSystem(int& i) noexcept : i_(i){};
      ForcingWithForcesForSpecificSystem(ForcingWithForcesForSpecificSystem &&
                                         other) noexcept
          : i_(other.i_) {}
      ~ForcingWithForcesForSpecificSystem() override = default;
      // overloaded for specialized systems
      template <typename TimeType>
      void apply_forces(TimeType /*time*/, int& /*system*/,
                        std::size_t /* index */) const {
        // do something
        ++i_;
      }
      template <typename TimeType>
      void apply_forces(TimeType /*time*/, float& /*system*/,
                        std::size_t /* index */) const {
        // do something
        --i_;
      }
      // no apply torques, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ForcingWithForcesForSpecificSystem";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica
/// [fwsf]

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Forcing.Forcing.with_forces_for_specific_system",
      "[Unit][Simulator][Forcing]") {
    int i = 2;
    using F = ::elastica::forcing::ForcingWithForcesForSpecificSystem<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_time = 3;
    std::size_t mock_index = 3;

    int mock_int_system = 2;
    float mock_float_system = 2.0F;

    // check-metaprogramming
    {
      static_assert(F::derived_has_apply_forces<decltype(mock_time),
                                                decltype(mock_int_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_apply_forces<decltype(mock_time),
                                                decltype(mock_float_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces<decltype(mock_time), double>(),
          "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_forces<decltype(mock_time), char>(),
                    "forcing metaprogramming failure!");

      static_assert(
          F::derived_has_no_apply_torques<decltype(mock_time),
                                          decltype(mock_int_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques<decltype(mock_time),
                                          decltype(mock_float_system)>(),
          "forcing metaprogramming failure!");

      static_assert(
          F::derived_has_no_apply_forces_on_range<decltype(mock_time),
                                                  decltype(mock_int_system)>(),
          "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_forces_on_range<
                        decltype(mock_time), decltype(mock_float_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time),
                                                   decltype(mock_int_system)>(),
          "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_torques_on_range<
                        decltype(mock_time), decltype(mock_float_system)>(),
                    "forcing metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      my_forcing.apply_forces(mock_time, mock_int_system, mock_index);
      const int expected1 = (2 + 2);
      CHECK(my_forcing.i_ == expected1);

      my_forcing.apply_forces(mock_time, mock_float_system, mock_index);
      const int expected2 = expected1 - 1;
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_forces_internal(mock_time, mock_int_system,
                                       mock_indices);
      const int expected3 = expected2 + 4;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_int_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_float_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_forces_internal(mock_time, mock_float_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_forces_internal(
          mock_time, mock_int_system, mock_indices.data(), mock_indices.size());
      const int expected3 = expected2 + 4;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(
          mock_time, mock_int_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_float_system,
                                        mock_indices.data(),
                                        mock_indices.size());
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_forces_internal(mock_time, mock_float_system,
                                       mock_indices.data(),
                                       mock_indices.size());
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_forcing.i_;
      double mock_double_system(5.0);
      my_forcing.apply_forces_internal(mock_time, mock_double_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected);

      char mock_char('a');
      my_forcing.apply_forces_internal(mock_time, mock_char, mock_indices);
      CHECK(my_forcing.i_ == expected);
    };

    // check apply forces for the above cases
    auto check_runtime_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_forcing.i_;
      double mock_double_system(5.0);
      my_forcing.apply_forces_internal(mock_time, mock_double_system,
                                       mock_indices.data(),
                                       mock_indices.size());
      CHECK(my_forcing.i_ == expected);

      char mock_char('a');
      my_forcing.apply_forces_internal(
          mock_time, mock_char, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == expected);
    };

    auto check_range = [&]() {
      // [1 to 9], 9 numbers
      auto mock_range = numeric_range(1UL, 10UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_forces_internal(mock_time, mock_int_system, mock_range);
      const int expected3 = expected2 + 9;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_int_system, mock_range);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_float_system,
                                        mock_range);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_forces_internal(mock_time, mock_float_system,
                                       mock_range);
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_range_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_range(1UL, 10UL);
      const int expected = my_forcing.i_;
      double mock_double_system(5.0);
      my_forcing.apply_forces_internal(mock_time, mock_double_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected);

      char mock_char('a');
      my_forcing.apply_forces_internal(mock_time, mock_char, mock_indices);
      CHECK(my_forcing.i_ == expected);
    };

    check_public_members();
    check_at();
    check_at_is_noop_for_nonoverloaded_types();
    check_runtime_at();
    check_runtime_at_is_noop_for_nonoverloaded_types();
    check_range();
    check_range_is_noop_for_nonoverloaded_types();
  }
}  // namespace

/// [fwtf]
namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithForcesForTemplatedSystems) {
     public:
      explicit ForcingWithForcesForTemplatedSystems(int& i) noexcept : i_(i){};
      ForcingWithForcesForTemplatedSystems(
          ForcingWithForcesForTemplatedSystems && other) noexcept
          : i_(other.i_) {}
      ~ForcingWithForcesForTemplatedSystems() override = default;
      // overloaded for specialized systems

      // in the base class we have <Time,System>
      // can we do partial specialization?
      template <typename TimeType, typename T>
      void apply_forces(TimeType /*time*/, std::vector<T>& /*system*/,
                        std::size_t /* index */) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename T>
      void apply_forces(TimeType /*time*/, std::list<T>& /*system*/,
                        std::size_t /* index */) const {
        // do something
        --i_;
      }

      // no apply torques, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ForcingWithForcesForTemplatedSystems";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica
/// [fwtf]

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Forcing.Forcing.with_forces_for_templated_system",
      "[Unit][Simulator][Forcing]") {
    int i = 2;

    using F = ::elastica::forcing::ForcingWithForcesForTemplatedSystems<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_time = 3;
    std::size_t mock_index = 3;

    std::vector<int> mock_vector_system{1, 2, 3};
    std::list<int> mock_list_system = {4, 5, 6};

    // check-metaprogramming
    {
      static_assert(F::derived_has_apply_forces<decltype(mock_time),
                                                decltype(mock_vector_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_apply_forces<decltype(mock_time),
                                                decltype(mock_list_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_forces<decltype(mock_time),
                                                   std::array<int, 4>>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_forces<decltype(mock_time),
                                                   std::deque<double>>(),
                    "forcing metaprogramming failure!");

      static_assert(
          F::derived_has_no_apply_torques<decltype(mock_time),
                                          decltype(mock_vector_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques<decltype(mock_time),
                                          decltype(mock_list_system)>(),
          "forcing metaprogramming failure!");

      static_assert(F::derived_has_no_apply_forces_on_range<
                        decltype(mock_time), decltype(mock_vector_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces_on_range<decltype(mock_time),
                                                  decltype(mock_list_system)>(),
          "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_torques_on_range<
                        decltype(mock_time), decltype(mock_vector_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_torques_on_range<
                        decltype(mock_time), decltype(mock_list_system)>(),
                    "forcing metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_forcing.apply_forces(mock_time, mock_vector_system, mock_index);
      my_forcing.apply_forces(mock_time, mock_vector_system, mock_index);
      const int expected1 = (2 + 2);
      CHECK(my_forcing.i_ == expected1);

      my_forcing.apply_forces(mock_time, mock_list_system, mock_index);
      const int expected2 = expected1 - 1;
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_forces_internal(mock_time, mock_vector_system,
                                       mock_indices);
      const int expected3 = expected2 + 4;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_vector_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_list_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_forces_internal(mock_time, mock_list_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_forcing.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_forcing.apply_forces_internal(mock_time, mock_array_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_forcing.apply_forces_internal(mock_time, mock_deque, mock_indices);
      CHECK(my_forcing.i_ == expected);
    };

    // check apply forces for the above cases
    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_forces_internal(mock_time, mock_vector_system,
                                       mock_indices.data(),
                                       mock_indices.size());
      const int expected3 = expected2 + 4;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_vector_system,
                                        mock_indices.data(),
                                        mock_indices.size());
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_list_system,
                                        mock_indices.data(),
                                        mock_indices.size());
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_forces_internal(mock_time, mock_list_system,
                                       mock_indices.data(),
                                       mock_indices.size());
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_runtime_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_forcing.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_forcing.apply_forces_internal(mock_time, mock_array_system,
                                       mock_indices.data(),
                                       mock_indices.size());
      CHECK(my_forcing.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_forcing.apply_forces_internal(
          mock_time, mock_deque, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == expected);
    };

    auto check_range = [&]() {
      // [1 to 9], 9 numbers
      auto mock_range = numeric_range(1UL, 10UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_forces_internal(mock_time, mock_vector_system,
                                       mock_range);
      const int expected3 = expected2 + 9;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_vector_system,
                                        mock_range);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_torques_internal(mock_time, mock_list_system,
                                        mock_range);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_forces_internal(mock_time, mock_list_system, mock_range);
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_range_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_range = numeric_range(1UL, 10UL);
      const int expected = my_forcing.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_forcing.apply_forces_internal(mock_time, mock_array_system,
                                       mock_range);
      CHECK(my_forcing.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_forcing.apply_forces_internal(mock_time, mock_deque, mock_range);
      CHECK(my_forcing.i_ == expected);
    };

    check_public_members();
    check_at();
    check_at_is_noop_for_nonoverloaded_types();
    check_runtime_at();
    check_runtime_at_is_noop_for_nonoverloaded_types();
    check_range();
    check_range_is_noop_for_nonoverloaded_types();
  }
}  // namespace

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithTorquesForSpecificSystem) {
     public:
      explicit ForcingWithTorquesForSpecificSystem(int& i) noexcept : i_(i){};
      ForcingWithTorquesForSpecificSystem(ForcingWithTorquesForSpecificSystem &&
                                          other) noexcept
          : i_(other.i_) {}
      ~ForcingWithTorquesForSpecificSystem() override = default;
      // overloaded for specialized systems
      template <typename TimeType>
      void apply_torques(TimeType /*time*/, int& /*system*/,
                         std::size_t /* index */) const {
        // do something
        ++i_;
      }
      template <typename TimeType>
      void apply_torques(TimeType /*time*/, float& /*system*/,
                         std::size_t /* index */) const {
        // do something
        --i_;
      }
      template <typename TimeType>
      void apply_torques_on_range(TimeType /*time*/, int& /*system*/,
                                  int /*start*/, int /*stop*/) const {
        // do something
        ++i_;
      }
      template <typename TimeType>
      void apply_torques_on_range(TimeType /*time*/, float& /*system*/,
                                  int /*start*/, int /*stop*/) const {
        // do something
        --i_;
      }

      // no apply torques, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ForcingWithTorquesForSpecificSystem";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Forcing.Forcing.with_torques_for_specific_system",
      "[Unit][Simulator][Forcing]") {
    int i = 2;
    using F = ::elastica::forcing::ForcingWithTorquesForSpecificSystem<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_time = 3;
    std::size_t mock_index = 3;

    int mock_int_system = 2;
    float mock_float_system = 2.0F;

    // check-metaprogramming
    {
      static_assert(F::derived_has_apply_torques<decltype(mock_time),
                                                 decltype(mock_int_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_apply_torques<decltype(mock_time),
                                                 decltype(mock_float_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques<decltype(mock_time), double>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques<decltype(mock_time), char>(),
          "forcing metaprogramming failure!");

      static_assert(F::derived_has_no_apply_forces<decltype(mock_time),
                                                   decltype(mock_int_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces<decltype(mock_time),
                                         decltype(mock_float_system)>(),
          "forcing metaprogramming failure!");

      static_assert(
          F::derived_has_apply_torques_on_range<decltype(mock_time),
                                                decltype(mock_int_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_apply_torques_on_range<decltype(mock_time),
                                                decltype(mock_float_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time),
                                                   double>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time), char>(),
          "forcing metaprogramming failure!");

      static_assert(
          F::derived_has_no_apply_forces_on_range<decltype(mock_time),
                                                  decltype(mock_int_system)>(),
          "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_forces_on_range<
                        decltype(mock_time), decltype(mock_float_system)>(),
                    "forcing metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_forcing.apply_torques(mock_time, mock_int_system, mock_index);
      my_forcing.apply_torques(mock_time, mock_int_system, mock_index);
      const int expected1 = (2 + 2);
      CHECK(my_forcing.i_ == expected1);

      my_forcing.apply_torques(mock_time, mock_float_system, mock_index);
      const int expected2 = expected1 - 1;
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_torques_internal(mock_time, mock_int_system,
                                        mock_indices);
      const int expected3 = expected2 + 4;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_int_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_float_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_torques_internal(mock_time, mock_float_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_forcing.i_;
      double mock_double_system(5.0);
      my_forcing.apply_torques_internal(mock_time, mock_double_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected);

      char mock_char('a');
      my_forcing.apply_torques_internal(mock_time, mock_char, mock_indices);
      CHECK(my_forcing.i_ == expected);
    };

    // check apply forces for the above cases
    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_torques_internal(
          mock_time, mock_int_system, mock_indices.data(), mock_indices.size());
      const int expected3 = expected2 + 4;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(
          mock_time, mock_int_system, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_float_system,
                                       mock_indices.data(),
                                       mock_indices.size());
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_torques_internal(mock_time, mock_float_system,
                                        mock_indices.data(),
                                        mock_indices.size());
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_runtime_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_forcing.i_;
      double mock_double_system(5.0);
      my_forcing.apply_torques_internal(mock_time, mock_double_system,
                                        mock_indices.data(),
                                        mock_indices.size());
      CHECK(my_forcing.i_ == expected);

      char mock_char('a');
      my_forcing.apply_torques_internal(
          mock_time, mock_char, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == expected);
    };

    auto check_discontiguous_range = [&]() {
      // [1 to 9], 5 numbers
      auto mock_range = numeric_range(1UL, 10UL, 2);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_torques_internal(mock_time, mock_int_system, mock_range);
      const int expected3 = expected2 + 5;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_int_system, mock_range);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_float_system,
                                       mock_range);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_torques_internal(mock_time, mock_float_system,
                                        mock_range);
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_range_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_range(1UL, 10UL);
      const int expected = my_forcing.i_;
      double mock_double_system(5.0);
      my_forcing.apply_torques_internal(mock_time, mock_double_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected);

      char mock_char('a');
      my_forcing.apply_torques_internal(mock_time, mock_char, mock_indices);
      CHECK(my_forcing.i_ == expected);
    };

    auto check_range = [&]() {
      // [1 to 9], 9 numbers
      // step size 1
      auto mock_range = numeric_range(1UL, 10UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_torques_internal(mock_time, mock_int_system, mock_range);
      const int expected3 = expected2 + 1;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_int_system, mock_range);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_float_system,
                                       mock_range);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_torques_internal(mock_time, mock_float_system,
                                        mock_range);
      CHECK(my_forcing.i_ == expected2);
    };

    check_public_members();
    check_at();
    check_at_is_noop_for_nonoverloaded_types();
    check_runtime_at();
    check_runtime_at_is_noop_for_nonoverloaded_types();
    check_discontiguous_range();
    check_range_is_noop_for_nonoverloaded_types();
    check_range();
  }
}  // namespace

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithTorquesForTemplatedSystems) {
     public:
      explicit ForcingWithTorquesForTemplatedSystems(int& i) noexcept : i_(i){};
      ForcingWithTorquesForTemplatedSystems(
          ForcingWithTorquesForTemplatedSystems && other) noexcept
          : i_(other.i_) {}
      ~ForcingWithTorquesForTemplatedSystems() override = default;
      // overloaded for specialized systems

      // in the base class we have <Time,System>
      // can we do partial specialization?
      template <typename TimeType, typename T>
      void apply_torques(TimeType /*time*/, std::vector<T>& /*system*/,
                         std::size_t /* index */) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename T>
      void apply_torques(TimeType /*time*/, std::list<T>& /*system*/,
                         std::size_t /* index */) const {
        // do something
        --i_;
      }
      template <typename TimeType, typename T>
      void apply_torques_on_range(TimeType /*time*/, std::vector<T>& /*system*/,
                                  int /* start*/, int /*stop*/) const {
        // do something
        ++i_;
      }
      template <typename TimeType, typename T>
      void apply_torques_on_range(TimeType /*time*/, std::list<T>& /*system*/,
                                  int /* start*/, int /*stop*/) const {
        // do something
        --i_;
      }

      // no apply torques, automatically assumed to be no-op
      static std::string name() noexcept {
        return "ForcingWithTorquesForTemplatedSystems";
      }

     public:  // can be private too!
      int& i_;
    };

  }  // namespace forcing

}  // namespace elastica

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Forcing.Forcing.with_torques_for_templated_system",
      "[Unit][Simulator][Forcing]") {
    int i = 2;
    using F = ::elastica::forcing::ForcingWithTorquesForTemplatedSystems<>;
    auto my_forcing(check_and_make_forcing<F>(i));
    int mock_time = 3;
    std::size_t mock_index = 3;

    std::vector<int> mock_vector_system{1, 2, 3};
    std::list<int> mock_list_system = {4, 5, 6};

    // check-metaprogramming
    {
      static_assert(
          F::derived_has_apply_torques<decltype(mock_time),
                                       decltype(mock_vector_system)>(),
          "forcing metaprogramming failure!");
      static_assert(F::derived_has_apply_torques<decltype(mock_time),
                                                 decltype(mock_list_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_torques<decltype(mock_time),
                                                    std::array<int, 4>>(),
                    "forcing metaprogramming failure!");
      static_assert(F::derived_has_no_apply_torques<decltype(mock_time),
                                                    std::deque<double>>(),
                    "forcing metaprogramming failure!");

      static_assert(
          F::derived_has_no_apply_forces<decltype(mock_time),
                                         decltype(mock_vector_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces<decltype(mock_time),
                                         decltype(mock_list_system)>(),
          "forcing metaprogramming failure!");

      static_assert(
          F::derived_has_apply_torques_on_range<decltype(mock_time),
                                                decltype(mock_vector_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_apply_torques_on_range<decltype(mock_time),
                                                decltype(mock_list_system)>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time),
                                                   std::array<int, 4>>(),
          "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_torques_on_range<decltype(mock_time),
                                                   std::deque<double>>(),
          "forcing metaprogramming failure!");

      static_assert(F::derived_has_no_apply_forces_on_range<
                        decltype(mock_time), decltype(mock_vector_system)>(),
                    "forcing metaprogramming failure!");
      static_assert(
          F::derived_has_no_apply_forces_on_range<decltype(mock_time),
                                                  decltype(mock_list_system)>(),
          "forcing metaprogramming failure!");
    }

    // check the public member
    auto check_public_members = [&]() {
      my_forcing.apply_torques(mock_time, mock_vector_system, mock_index);
      my_forcing.apply_torques(mock_time, mock_vector_system, mock_index);
      const int expected1 = (2 + 2);
      CHECK(my_forcing.i_ == expected1);

      my_forcing.apply_torques(mock_time, mock_list_system, mock_index);
      const int expected2 = expected1 - 1;
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_torques_internal(mock_time, mock_vector_system,
                                        mock_indices);
      const int expected3 = expected2 + 4;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_vector_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_list_system,
                                       mock_indices);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_torques_internal(mock_time, mock_list_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_forcing.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_forcing.apply_torques_internal(mock_time, mock_array_system,
                                        mock_indices);
      CHECK(my_forcing.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_forcing.apply_torques_internal(mock_time, mock_deque, mock_indices);
      CHECK(my_forcing.i_ == expected);
    };

    auto check_discontiguous_range = [&]() {
      // [1 to 9], 9 numbers
      auto mock_range = numeric_range(1UL, 10UL, 2);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_torques_internal(mock_time, mock_vector_system,
                                        mock_range);
      const int expected3 = expected2 + 5;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_vector_system,
                                       mock_range);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_list_system, mock_range);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_torques_internal(mock_time, mock_list_system,
                                        mock_range);
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_runtime_at = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_torques_internal(mock_time, mock_vector_system,
                                        mock_indices.data(),
                                        mock_indices.size());
      const int expected3 = expected2 + 4;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_vector_system,
                                       mock_indices.data(),
                                       mock_indices.size());
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_list_system,
                                       mock_indices.data(),
                                       mock_indices.size());
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_torques_internal(mock_time, mock_list_system,
                                        mock_indices.data(),
                                        mock_indices.size());
      CHECK(my_forcing.i_ == expected2);
    };

    // check apply forces for the above cases
    auto check_runtime_at_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_indices = numeric_at(1UL, 2UL, 3UL, 4UL);
      const int expected = my_forcing.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_forcing.apply_torques_internal(mock_time, mock_array_system,
                                        mock_indices.data(),
                                        mock_indices.size());
      CHECK(my_forcing.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_forcing.apply_torques_internal(
          mock_time, mock_deque, mock_indices.data(), mock_indices.size());
      CHECK(my_forcing.i_ == expected);
    };

    // check apply forces for the above cases
    auto check_range_is_noop_for_nonoverloaded_types = [&]() {
      auto mock_range = numeric_range(1UL, 10UL);
      const int expected = my_forcing.i_;
      std::array<int, 5> mock_array_system = {1, 2, 3, 4, 5};
      my_forcing.apply_torques_internal(mock_time, mock_array_system,
                                        mock_range);
      CHECK(my_forcing.i_ == expected);

      std::deque<int> mock_deque = {1, 2, 3, 4, 5};
      my_forcing.apply_torques_internal(mock_time, mock_deque, mock_range);
      CHECK(my_forcing.i_ == expected);
    };

    auto check_range = [&]() {
      // [1 to 9], 9 numbers
      auto mock_range = numeric_range(1UL, 10UL);
      const int expected2 = my_forcing.i_;
      my_forcing.apply_torques_internal(mock_time, mock_vector_system,
                                        mock_range);
      const int expected3 = expected2 + 1;
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_vector_system,
                                       mock_range);
      CHECK(my_forcing.i_ == expected3);

      // noop
      my_forcing.apply_forces_internal(mock_time, mock_list_system, mock_range);
      CHECK(my_forcing.i_ == expected3);

      // check apply forces for the above cases
      my_forcing.apply_torques_internal(mock_time, mock_list_system,
                                        mock_range);
      CHECK(my_forcing.i_ == expected2);
    };

    check_public_members();
    check_at();
    check_at_is_noop_for_nonoverloaded_types();
    check_runtime_at();
    check_runtime_at_is_noop_for_nonoverloaded_types();
    check_discontiguous_range();
    check_range_is_noop_for_nonoverloaded_types();
    check_range();
  }
}  // namespace
