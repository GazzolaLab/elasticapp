//
// Created by Tejaswin Parthasarathy on 12/29/20.
//

#include <functional>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Constraints/Constraint.hpp"
#include "Simulator/Interface/Constraints/ConstraintBase.hpp"
#include "Simulator/Interface/Indexers/Range.hpp"
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

// make a constraint
#include "Simulator/Constraints/Constraint.hpp"
#include "Simulator/Constraints/MakeConstraint.hpp"
#include "Simulator/Constraints/Registration.hpp"
#include "Simulator/Interface/Constraints/MockConstraints.hpp"
#include "Utilities/Registration.hpp"

/// mocks
#include "Simulator/Interface/MockSystems.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::interface::constraints;

  using ::mock::MockSystem;
  using ::mock::TestConstraint;
  using ::mock::TestConstraintImpl;

  namespace {

    using RegistrarListCheck = tmpl::list<int, float, char>;
    using SystemType = float;
    static_assert(
        std::is_base_of<ConstraintBase<RegistrarListCheck>,
                        Constraint<SystemType, RegistrarListCheck>>::value,
        "Base violation!");

  }  // namespace

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Constraint.constructor",
      "[Unit][Simulator][Interface][Constraints]") {
    {
      using RegistrarList = tmpl::list<>;
      using ConcreteConstraint =
          TestConstraintImpl<std::reference_wrapper<int>, RegistrarList>;
      int i(2);
      std::size_t index(6UL);
      ConcreteConstraint cc(index, i, "test");
      CHECK(cc.concrete_system_type() == "int");
    }

    {
      using RegistrarList = tmpl::list<int, float, char>;
      using ConcreteConstraint =
          TestConstraintImpl<std::reference_wrapper<float>, RegistrarList>;
      float f(4.0F);
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.concrete_system_type() == "float");
    }

    {
      using RegistrarList = tmpl::list<char>;
      using ConcreteConstraint =
          TestConstraintImpl<std::reference_wrapper<double>, RegistrarList>;
      double d{5.12312};
      ConcreteConstraint cc(8UL, d, "testing");
      CHECK(cc.concrete_system_type() == "double");
    }
  }

}  // namespace

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintDefaulted) {
     public:
      ConstraintDefaulted() = default;
      ConstraintDefaulted(ConstraintDefaulted &&) noexcept {};
      // move constructor is deleted if a destructor is defined
      //      ~ConstraintDefaulted(){
      //        std::cout << "Destructor caleld!" << std::endl;
      //      }

      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System& /* system */,
                           std::size_t /* index */) const {}

      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System& /* system */,
                                std::size_t /* index */) const {}

      static std::string name() { return "ConstraintDefault"; }
    };

  }  // namespace constraints

}  // namespace elastica

struct ObjectCount : counter<ObjectCount> {
  /*  ReferenceCount() = default;
    ReferenceCount(ReferenceCount const& other) : count(other.count + 1) {}
    ReferenceCount(ReferenceCount&& other) noexcept : count(other.count + 1) {}
    unsigned int count = 0U;*/
};

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintReferenceCounted) {
     public:
      explicit ConstraintReferenceCounted() : r(){};
      ConstraintReferenceCounted(ConstraintReferenceCounted && other) noexcept
          : r(std::move(other.r)){};
      // move constructor is deleted if a destructor is defined
      ~ConstraintReferenceCounted() override = default;
      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System& /* system */,
                           std::size_t /* index */) const {}

      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System& /* system */,
                                std::size_t /* index */) const {}

      static std::string name() { return "ConstraintRefCounted"; }

      ObjectCount r;
    };

  }  // namespace constraints

}  // namespace elastica

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithFundamentalTypes) {
     public:
      ConstraintWithFundamentalTypes(int a, float b, char c, int& d, int* p)
          : a_(a), b_(b), c_(c), d_(d), p_(p){};
      ConstraintWithFundamentalTypes(ConstraintWithFundamentalTypes &&
                                     other) noexcept
          : a_(other.a_),
            b_(other.b_),
            c_(other.c_),
            d_(other.d_),
            p_(other.p_) {}
      ~ConstraintWithFundamentalTypes() override = default;

      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System& /* system */,
                           std::size_t /* index */) const {}

      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System& /* system */,
                                std::size_t /* index */) const {}

      static std::string name() noexcept { return "FundamentalConstraint"; }

     private:
      int a_;
      float b_;
      char c_;
      int& d_;
      int* p_;
    };

  }  // namespace constraints

}  // namespace elastica

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithAggregateTypes) {
     public:
      using ST = std::string;
      ConstraintWithAggregateTypes(ST s1, ST s2, ST s3)
          : s_{std::move(s1), std::move(s2), std::move(s3)} {};
      ConstraintWithAggregateTypes(ConstraintWithAggregateTypes &&
                                   other) noexcept
          : s_(std::move(other.s_)) {}
      ~ConstraintWithAggregateTypes() override = default;

      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System& /* system */,
                           std::size_t /* index */) const {}

      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System& /* system */,
                                std::size_t /* index */) const {}

      static std::string name() { return "AggregateConstraint"; }

     private:
      std::vector<ST> s_;
    };

  }  // namespace constraints

}  // namespace elastica

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintWithNonCopyableTypes) {
     public:
      using ST = std::unique_ptr<float>;
      ConstraintWithNonCopyableTypes(ST && s1, ST && s2, ST && s3) {
        s_.emplace_back(std::move(s1));
        s_.emplace_back(std::move(s2));
        s_.emplace_back(std::move(s3));
      }
      ConstraintWithNonCopyableTypes(ConstraintWithNonCopyableTypes &&
                                     other) noexcept
          : s_(std::move(other.s_)) {}
      ~ConstraintWithNonCopyableTypes() override = default;

      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System& /* system */,
                           std::size_t /* index */) const {}

      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System& /* system */,
                                std::size_t /* index */) const {}

      static std::string name() { return "AggregateConstraint"; }

     private:
      std::vector<ST> s_;
    };

  }  // namespace constraints

}  // namespace elastica

// finally one with make_constraint

auto internal_constraint_first IGNORE_UNUSED = [](auto...) -> void {};
using ConstraintFromMakeConstraint =
    decltype(::elastica::constraints::make_constraint(
        ::elastica::constraints::make_value_constraint(
            internal_constraint_first),
        ::elastica::constraints::make_rate_constraint(
            internal_constraint_first))());

namespace {

  struct Checks {
    using CC = TestConstraint<std::reference_wrapper<MockSystem>>;

    template <typename T>
    struct CheckConformantConstraint : std::true_type {
      static_assert(CC::conformant_constraint<T>(),
                    "A conformant constraint was recognized as non-conformant");
    };

    template <typename T>
    struct CheckNonConformantConstraint : std::true_type {
      static_assert(CC::non_conformant_constraint<T>(),
                    "A non conformant constraint was recognized as conformant");
    };

    template <typename T>
    struct CheckConformantIndexSet : std::true_type {
      static_assert(CC::conformant_index_set<T>(),
                    "A conformant index set was recognized as non-conformant");
    };

    template <typename T>
    struct CheckNonConformantIndexSet : std::true_type {
      static_assert(CC::non_conformant_index_set<T>(),
                    "A non conformant index set was recognized as conformant");
    };

    template <typename>
    struct CheckConformantInterface;

    template <typename C, typename I>
    struct CheckConformantInterface<tmpl::list<C, I>> : std::true_type {
      static_assert(CC::conformant_interface<C, I>(),
                    "A conformant interface was recognized as non-conformant");
    };

    template <typename>
    struct CheckNonConformantInterface;

    template <typename C, typename I>
    struct CheckNonConformantInterface<tmpl::list<C, I>> : std::true_type {
      static_assert(CC::non_conformant_interface<C, I>(),
                    "A non-conformant interface was recognized as conformant");
    };
  };

  using ConstraintsList =
      tmpl::list<::elastica::constraints::ConstraintDefaulted<>,
                 ::elastica::constraints::ConstraintReferenceCounted<>,
                 ::elastica::constraints::ConstraintWithFundamentalTypes<>,
                 ::elastica::constraints::ConstraintWithAggregateTypes<>,
                 ::elastica::constraints::ConstraintWithNonCopyableTypes<>,
                 ConstraintFromMakeConstraint>;
  using NonConstraintsList = tmpl::list<int, float*, const char, volatile bool>;

  using conformant_constraints_checks IGNORE_UNUSED =
      decltype(tmpl::transform<ConstraintsList,
                               Checks::CheckConformantConstraint<tmpl::_1>>{});
  using non_conformant_constraints_checks IGNORE_UNUSED = decltype(
      tmpl::transform<NonConstraintsList,
                      Checks::CheckNonConformantConstraint<tmpl::_1>>{});

  using IndexSetList =
      tmpl::list<decltype(::elastica::interface::at(1UL, 2UL)),
                 decltype(::elastica::interface::at(1UL, 5UL, 10UL, end - 2))>;
  using NonIndexSetList =
      tmpl::list<unsigned char, int, short, long,
                 decltype(::elastica::interface::range(1, 6, 1))>;

  using conformant_indexset_checks IGNORE_UNUSED =
      decltype(tmpl::transform<IndexSetList,
                               Checks::CheckConformantIndexSet<tmpl::_1>>{});
  using non_conformant_indexset_checks IGNORE_UNUSED =
      decltype(tmpl::transform<NonIndexSetList,
                               Checks::CheckNonConformantIndexSet<tmpl::_1>>{});

  using InterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<ConstraintsList, IndexSetList>{});
  using FirstNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<NonConstraintsList, IndexSetList>{});
  using SecondNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<ConstraintsList, NonIndexSetList>{});
  using ThirdNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<NonConstraintsList, NonIndexSetList>{});

  using conformant_interface_checks IGNORE_UNUSED =
      decltype(tmpl::transform<InterfaceList,
                               Checks::CheckConformantInterface<tmpl::_1>>{});
  using first_conformant_interface_checks IGNORE_UNUSED = decltype(
      tmpl::transform<FirstNonInterfaceList,
                      Checks::CheckNonConformantInterface<tmpl::_1>>{});
  using second_conformant_interface_checks IGNORE_UNUSED = decltype(
      tmpl::transform<SecondNonInterfaceList,
                      Checks::CheckNonConformantInterface<tmpl::_1>>{});
  using third_conformant_interface_checks IGNORE_UNUSED = decltype(
      tmpl::transform<ThirdNonInterfaceList,
                      Checks::CheckNonConformantInterface<tmpl::_1>>{});

}  // namespace

namespace {

  void test_constraint_with_default() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;

    MockSystem f{100};

    {  // rvalue
      ConcreteConstraint cc(6UL, f, "test");
      ::elastica::constraints::ConstraintDefaulted<> constraint{};
      CHECK(cc.is_not_valid());
      cc.with(std::move(constraint), ::elastica::interface::at(1UL));
      CHECK(f.n_indices == 100);
      CHECK(not cc.is_not_valid());
    }

    {  // rvalue
      ConcreteConstraint cc(6UL, f, "test");
      ::elastica::constraints::ConstraintDefaulted<> constraint{};
      auto x = ::elastica::interface::at(1UL, end - 2);
      CHECK(cc.is_not_valid());
      cc.with(std::move(constraint), x);
      CHECK(f.n_indices == 100);
      CHECK(not cc.is_not_valid());
    }

    {  // xvalue
      ConcreteConstraint cc(6UL, f, "test");
      cc.with(::elastica::constraints::ConstraintDefaulted<>{},
              ::elastica::interface::at(2UL));
      CHECK(f.n_indices == 100);
      CHECK(not cc.is_not_valid());
    }
  }

  void test_constraint_with_object_counts() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    {  // prvalue
      {
        ConcreteConstraint cc(6UL, f, "test");
        CHECK(cc.is_not_valid());
        cc.with(::elastica::constraints::ConstraintReferenceCounted<>{},
                ::elastica::interface::at(2UL));
        CHECK(f.n_indices == 100);
        CHECK(not cc.is_not_valid());
        // While the contents are moved around, three empty constraints are
        // created and destroyed, 1 more constraint is within

        CHECK(counter<ObjectCount>::objects_alive ==
              1);  // 1 object is alive within cc
        CHECK(counter<ObjectCount>::objects_created ==
              4);                                          // 4 objects in total
        CHECK(counter<ObjectCount>::objects_copied == 0);  // ensure no copies
        CHECK(counter<ObjectCount>::objects_moved == 3);   // 4 -1 ?
      }

      CHECK(counter<ObjectCount>::objects_alive ==
            0);  // After cc dies, the object should too!
    }

    {  // xvalue
      {
        ConcreteConstraint cc(6UL, f, "test");
        ::elastica::constraints::ConstraintReferenceCounted<> constraint{};
        CHECK(cc.is_not_valid());
        cc.with(std::move(constraint), ::elastica::interface::at(1UL));
        CHECK(f.n_indices == 100);
        CHECK(not cc.is_not_valid());
        // While the contents are moved around, three empty constraints are
        // created and destroyed, 1 more constraint is within

        CHECK(counter<ObjectCount>::objects_alive ==
              2);  // 1 object is alive within cc, 1 is in the current scope
        CHECK(counter<ObjectCount>::objects_created ==
              4 + 4);  // 4 objects in total, adding on to the previous 4
        CHECK(counter<ObjectCount>::objects_copied == 0);  // ensure no copies
        CHECK(counter<ObjectCount>::objects_moved == 3 + 3);  // 4 -1 ?
      }

      CHECK(counter<ObjectCount>::objects_alive ==
            0);  // After cc dies, the object should too!
    }
  }

  void test_constraint_with_fundamental_types() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      ::elastica::constraints::ConstraintWithFundamentalTypes<> constraint{
          1, 2.0F, 'c', i, &j};
      CHECK(f.n_indices == 100);
      cc.with(std::move(constraint), ::elastica::interface::at(1UL, 4UL, 3UL));
      CHECK(not cc.is_not_valid());
    }

    {  // xvalue
      int i(2);
      int j(4);
      ConcreteConstraint cc(6UL, f, "test");
      cc.with(
          ::elastica::constraints::ConstraintWithFundamentalTypes<>{1, 2.0F,
                                                                    'c', i, &j},
          ::elastica::interface::at(3UL, 2UL, 1UL));
      CHECK(f.n_indices == 100);
      CHECK(not cc.is_not_valid());
    }
  }

  void test_constraint_with_aggregate_types() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    {  // rvalue
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      ::elastica::constraints::ConstraintWithAggregateTypes<> constraint{
          "ODIs", "are", "good"};
      CHECK(f.n_indices == 100);
      cc.with(std::move(constraint),
              ::elastica::interface::at(1UL, 4UL, 3UL, end - 20));
      CHECK(not cc.is_not_valid());
    }

    {  // xvalue
      ConcreteConstraint cc(6UL, f, "test");
      cc.with(
          ::elastica::constraints::ConstraintWithAggregateTypes<>{
              "Tests", "are", "better"},
          ::elastica::interface::at(3UL, 2UL, 1UL));
      CHECK(f.n_indices == 100);
      CHECK(not cc.is_not_valid());
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.Constraint.record",
                     "[Unit][Simulator][Interface][Constraints]") {
    using RegistrarList = tmpl::list<char>;
    using ConstraintInterface =
        TestConstraintImpl<std::reference_wrapper<double>, RegistrarList>;
    double d{5.12312};
    {
      ConstraintInterface cc(8UL, d, "testing");
      cc.record_name_of<int>();
      CHECK(cc.concrete_constraint_type() == "int");
      cc.record_name_of<float>();
      CHECK(cc.concrete_constraint_type() == "float");
      cc.record_name_of<::elastica::constraints::ConstraintDefaulted<>>();
      CHECK(cc.concrete_constraint_type() == "ConstraintDefaulted");
    }

    // test the with interface for recording
    {
      ConstraintInterface cc(8UL, d, "testing");
      ::elastica::constraints::ConstraintDefaulted<> constraint{};
      cc.with(std::move(constraint));
      CHECK(cc.concrete_constraint_type() == "ConstraintDefaulted");
    }
    {
      ConstraintInterface cc(8UL, d, "testing");
      cc.with<::elastica::constraints::ConstraintDefaulted<>>();
      CHECK(cc.concrete_constraint_type() == "ConstraintDefaulted");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.Constraint.set_name",
                     "[Unit][Simulator][Interface][Constraints]") {
    using RegistrarList = tmpl::list<char>;
    using ConstraintInterface =
        TestConstraintImpl<std::reference_wrapper<double>, RegistrarList>;
    double d{5.12312};
    {
      ConstraintInterface cc(8UL, d, "testing");
      CHECK(cc.id().name == "testing");
      cc.set_name("boo!");
      CHECK(cc.id().name == "boo!");
    }
    {
      ConstraintInterface cc(8UL, d, "testing");
      CHECK(cc.id().name == "testing");
      // cc.with_name("boo!");
      // CHECK(cc.id().name == "boo!");
    }
    {
      CHECK(ConstraintInterface(8UL, d, "testing")
                // .set_name("boo!")
                .id()
                .name == "testing");
    }
    {
      CHECK(ConstraintInterface(8UL, d, "testing")
                // .with_name("boo!")
                .id()
                .name == "testing");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Constraints.Constraint.with",
                     "[Unit][Simulator][Interface][Constraints]") {
    test_constraint_with_default();
    test_constraint_with_object_counts();
    test_constraint_with_fundamental_types();
    test_constraint_with_aggregate_types();
  }

  void test_constraint_with_fundamental_types_via_forwarding_with() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      ::elastica::constraints::ConstraintWithFundamentalTypes<> constraint{
          1, 2.0F, 'c', i, &j};
      CHECK(f.n_indices == 100);
      auto x = cc.with(std::move(constraint));
      // shouldnt be valid in this intermediate state
      CHECK(cc.is_not_valid());
      // but has a valid name
      CHECK(cc.concrete_constraint_type() == "ConstraintWithFundamentalTypes");
      x.at(1UL, 2UL, 3UL, 4UL);
      CHECK(not cc.is_not_valid());
      CHECK(f.n_indices == 100);
    }

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      ::elastica::constraints::ConstraintWithFundamentalTypes<> constraint{
          1, 2.0F, 'c', i, &j};
      // intended usage
      cc.with(std::move(constraint)).at(1UL, 2UL, 3UL, 4UL);
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_constraint_type() == "ConstraintWithFundamentalTypes");
      CHECK(f.n_indices == 100);
    }

    {  // xvalue
      int i(2);
      int j(4);
      ConcreteConstraint cc(6UL, f, "test");
      cc.with(::elastica::constraints::ConstraintWithFundamentalTypes<>{
                  1, 2.0F, 'c', i, &j})
          .at(1UL, 2UL, 3UL);
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_constraint_type() == "ConstraintWithFundamentalTypes");
    }
  }

  void test_constraint_with_aggregate_types_via_forwarding_with() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    {  // rvalue
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      ::elastica::constraints::ConstraintWithAggregateTypes<> constraint{
          "ODIs", "are", "good"};
      cc.with(std::move(constraint)).at(1UL, 4UL, 3UL);
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_constraint_type() == "ConstraintWithAggregateTypes");
      CHECK(f.n_indices == 100);
    }

    {  // xvalue
      ConcreteConstraint cc(6UL, f, "test");
      cc.with(::elastica::constraints::ConstraintWithAggregateTypes<>{
                  "Tests", "are", "better"})
          .at(3UL, 2UL, 1UL);
      CHECK(cc.concrete_constraint_type() == "ConstraintWithAggregateTypes");
      CHECK(f.n_indices == 100);
      CHECK(not cc.is_not_valid());
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Constraint.forwarding_with",
      "[Unit][Simulator][Interface][Constraints]") {
    test_constraint_with_fundamental_types_via_forwarding_with();
    test_constraint_with_aggregate_types_via_forwarding_with();
  }

  void test_constraint_with_fundamental_types_via_forwarded_arguments() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      CHECK(f.n_indices == 100);
      auto x =
          cc.with<::elastica::constraints::ConstraintWithFundamentalTypes<>>(
              1, 2.0F, 'c', i, &j);
      // shouldnt be valid in this intermediate state
      CHECK(cc.is_not_valid());
      // but has a valid name
      CHECK(cc.concrete_constraint_type() == "ConstraintWithFundamentalTypes");
      x.at(1UL, 2UL, 3UL, 4UL);
      CHECK(not cc.is_not_valid());
      CHECK(f.n_indices == 100);
    }

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      // intended usage
      cc.with<::elastica::constraints::ConstraintWithFundamentalTypes<>>(
            1, 2.0F, 'c', i, &j)
          .at(1UL, 2UL, 3UL, 4UL);
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_constraint_type() == "ConstraintWithFundamentalTypes");
      CHECK(f.n_indices == 100);
    }
  }

  void test_constraint_with_aggregate_types_via_forwarded_arguments() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    {  // rvalue
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      cc.with<::elastica::constraints::ConstraintWithAggregateTypes<>>(
            "ODIs", "are", "good")
          .at(1UL, 4UL, 3Ul);
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_constraint_type() == "ConstraintWithAggregateTypes");
      CHECK(f.n_indices == 100);
    }

    {  // xvalue
      ConcreteConstraint cc(6UL, f, "test");
      auto x = cc.with<::elastica::constraints::ConstraintWithAggregateTypes<>>(
          "Tests", "are", "better");
      CHECK(cc.is_not_valid());
      CHECK(cc.concrete_constraint_type() == "ConstraintWithAggregateTypes");
      x.at(3UL, 2UL, 1UL);
      CHECK(f.n_indices == 100);
      CHECK(not cc.is_not_valid());
    }
  }

  void test_constraint_with_noncopyable_types_via_forwarded_arguments() {
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    {  // rvalue
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      // float r(2.0F), p(5.000F), q(8.000F);
      cc.with<::elastica::constraints::ConstraintWithNonCopyableTypes<>>(
            std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
            std::make_unique<float>(80.0F))
          .at(1UL, 4UL, 3UL);
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_constraint_type() == "ConstraintWithNonCopyableTypes");
      CHECK(f.n_indices == 100);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Constraint.with_forwarded_arguments",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    test_constraint_with_fundamental_types_via_forwarded_arguments();
    test_constraint_with_aggregate_types_via_forwarded_arguments();
    test_constraint_with_noncopyable_types_via_forwarded_arguments();
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Constraint.exception_for_incorrect_indices",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    {  // rvalue
      ConcreteConstraint cc(6UL, f, "test");

      CHECK_THROWS_NESTED_WITH(
          cc.unsafe_with(
              ::elastica::constraints::ConstraintWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(1UL, 4UL, 143UL, end - 150)),
          "Attempts to constrain system",
          "Number of degrees of freedom exceeded");

      ConcreteConstraint new_cc(6UL, f, "test");

      CHECK_THROWS_NESTED_WITH(
          cc.unsafe_with(
              ::elastica::constraints::ConstraintWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(1UL, 4UL, 143UL, end - 150)),
          "Attempts to constrain system",
          "Number of degrees of freedom exceeded");
    }

    {  // repeated indices

      ConcreteConstraint cc(6UL, f, "test");
      CHECK_THROWS_NESTED_WITH(
          cc.unsafe_with(
              ::elastica::constraints::ConstraintWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(1UL, 2UL, 2UL, end - 150)),
          "Attempts to constrain system",
          "Number of degrees of freedom exceeded");

      // std::logic_error);

      ConcreteConstraint new_cc(6UL, f, "test");
      CHECK_THROWS_NESTED_WITH(
          new_cc.unsafe_with(
              ::elastica::constraints::ConstraintWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(1UL, 2UL, 2UL, end - 150)),
          "Attempts to constrain system",
          "Number of degrees of freedom exceeded");
    }
  }

  template <template <typename> class ConstraintType, typename... Args>
  std::unique_ptr<::elastica::constraints::detail::ConstraintBase<int>>
  make_constraint(Args&&... args) {
    return std::make_unique<ConstraintType<int>>(std::forward<Args>(args)...);
  }

  template <typename T, typename... Ints>
  std::unique_ptr<::elastica::interface::detail::At_base_t> make_at(
      T&& x, Ints&&... ints) {
    using at_t = decltype(::elastica::interface::at(
        std::forward<T>(x), std::forward<Ints>(ints)...));
    return std::unique_ptr<at_t>(new at_t(::elastica::interface::at(
        std::forward<T>(x), std::forward<Ints>(ints)...)));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Constraints.Constraint.with_unique_ptr",
      "[Unit][Simulator][Interface][Constraints]") {
    // clang-format on
    using ConcreteConstraint =
        TestConstraint<std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConstraint cc(6UL, f, "test");
      CHECK(cc.is_not_valid());
      // registar type doesnt really matter here
      //      std::unique_ptr<::elastica::constraints::detail::ConstraintBase<int>>
      //          runtime_constraint = std::make_unique<
      //              ::elastica::constraints::ConstraintWithFundamentalTypes<int>>(
      //              1, 2.0F, 'c', i, &j);
      auto runtime_constraint = make_constraint<
          ::elastica::constraints::ConstraintWithFundamentalTypes>(1, 2.0F, 'c',
                                                                   i, &j);
      CHECK(f.n_indices == 100);
      cc.with(std::move(runtime_constraint), make_at(1UL, 4UL, 3UL));
      CHECK(not cc.is_not_valid());
    }
  }

}  // namespace
