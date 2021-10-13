//
// Created by Tejaswin Parthasarathy on 1/26/21.
//

#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Forcing/Forcing.hpp"
#include "Simulator/Interface/Forcing/ForcingBase.hpp"
#include "Simulator/Interface/Indexers/Range.hpp"
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

// make a forcing
#include "Simulator/Forcing/Forcing.hpp"
#include "Simulator/Forcing/MakeForcing.hpp"
#include "Simulator/Forcing/Registration.hpp"
#include "Simulator/Interface/Forcing/MockForcing.hpp"
#include "Utilities/Registration.hpp"

/// mocks
#include "Simulator/Interface/MockSystems.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::interface::forcing;

  using ::mock::MockSystem;
  using ::mock::TestForcing;
  using ::mock::TestForcingImpl;

  namespace {

    using RegistrarListCheck = tmpl::list<int, float, char>;
    using SystemType = float;
    static_assert(
        std::is_base_of<ForcingBase<RegistrarListCheck>,
                        Forcing<SystemType, RegistrarListCheck>>::value,
        "Base violation!");

  }  // namespace

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.Forcing.constructor",
                     "[Unit][Simulator][Interface][Forcing]") {
    {
      using RegistrarList = tmpl::list<>;
      using ConcreteForcing = TestForcingImpl<int, RegistrarList>;
      int i(2);
      std::size_t index(6UL);
      ConcreteForcing cf(index, i, "test");
    }

    {
      using RegistrarList = tmpl::list<int, float, char>;
      using ConcreteForcing = TestForcingImpl<float, RegistrarList>;
      float f(4.0F);
      ConcreteForcing cf(6UL, f, "test");
    }

    {
      using RegistrarList = tmpl::list<char>;
      using ConcreteForcing = TestForcingImpl<double, RegistrarList>;
      double d{5.12312};
      ConcreteForcing cf(8UL, d, "testing");
      CHECK(cf.concrete_system_type() == "double");
    }
  }

}  // namespace

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingDefaulted) {
     public:
      ForcingDefaulted() = default;
      ForcingDefaulted(ForcingDefaulted &&) noexcept {};
      // move constructor is deleted if a destructor is defined
      //      ~ForcingDefaulted(){
      //        std::cout << "Destructor caleld!" << std::endl;
      //      }

      template <typename TimeType, typename System>
      void apply_torques(TimeType /* time */, System& /* system */,
                         int /* index */) const {}

      template <typename TimeType, typename System>
      void apply_forces(TimeType /* time */, System& /* system */,
                        int /* index */) const {}

      static std::string name() { return "ForcingDefault"; }
    };

  }  // namespace forcing

}  // namespace elastica

struct TestForcingObjectCount : counter<TestForcingObjectCount> {
  /*  ReferenceCount() = default;
    ReferenceCount(ReferenceCount const& other) : count(other.count + 1) {}
    ReferenceCount(ReferenceCount&& other) noexcept : count(other.count + 1) {}
    unsigned int count = 0U;*/
};

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingReferenceCounted) {
     public:
      explicit ForcingReferenceCounted() : r(){};
      ForcingReferenceCounted(ForcingReferenceCounted && other) noexcept
          : r(std::move(other.r)){};
      // move constructor is deleted if a destructor is defined
      ~ForcingReferenceCounted() override = default;
      template <typename TimeType, typename System>
      void apply_torques(TimeType /* time */, System& /* system */,
                         int /* index */) const {}

      template <typename TimeType, typename System>
      void apply_forces(TimeType /* time */, System& /* system */,
                        int /* index */) const {}

      static std::string name() { return "ForcingRefCounted"; }

      TestForcingObjectCount r;
    };

  }  // namespace forcing

}  // namespace elastica

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithFundamentalTypes) {
     public:
      ForcingWithFundamentalTypes(int a, float b, char c, int& d, int* p)
          : a_(a), b_(b), c_(c), d_(d), p_(p){};
      ForcingWithFundamentalTypes(ForcingWithFundamentalTypes && other) noexcept
          : a_(other.a_),
            b_(other.b_),
            c_(other.c_),
            d_(other.d_),
            p_(other.p_) {}
      ~ForcingWithFundamentalTypes() override = default;

      template <typename TimeType, typename System>
      void apply_torques(TimeType /* time */, System& /* system */,
                         int /* index */) const {}

      template <typename TimeType, typename System>
      void apply_forces(TimeType /* time */, System& /* system */,
                        int /* index */) const {}

      static std::string name() noexcept { return "FundamentalForcing"; }

     private:
      int a_;
      float b_;
      char c_;
      int& d_;
      int* p_;
    };

  }  // namespace forcing

}  // namespace elastica

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithAggregateTypes) {
     public:
      using ST = std::string;
      ForcingWithAggregateTypes(ST s1, ST s2, ST s3)
          : s_{std::move(s1), std::move(s2), std::move(s3)} {};
      ForcingWithAggregateTypes(ForcingWithAggregateTypes && other) noexcept
          : s_(std::move(other.s_)) {}
      ~ForcingWithAggregateTypes() override = default;

      template <typename TimeType, typename System>
      void apply_torques(TimeType /* time */, System& /* system */,
                         int /* index */) const {}

      template <typename TimeType, typename System>
      void apply_forces(TimeType /* time */, System& /* system */,
                        int /* index */) const {}

      static std::string name() { return "AggregateForcing"; }

     private:
      std::vector<ST> s_;
    };

  }  // namespace forcing

}  // namespace elastica

namespace elastica {

  namespace forcing {

    ELASTICA_DEFINE_FORCING(ForcingWithNonCopyableTypes) {
     public:
      using ST = std::unique_ptr<float>;
      ForcingWithNonCopyableTypes(ST && s1, ST && s2, ST && s3) {
        s_.emplace_back(std::move(s1));
        s_.emplace_back(std::move(s2));
        s_.emplace_back(std::move(s3));
      }
      ForcingWithNonCopyableTypes(ForcingWithNonCopyableTypes && other) noexcept
          : s_(std::move(other.s_)) {}
      ~ForcingWithNonCopyableTypes() override = default;

      template <typename TimeType, typename System>
      void apply_torques(TimeType /* time */, System& /* system */,
                         int /* index */) const {}

      template <typename TimeType, typename System>
      void apply_forces(TimeType /* time */, System& /* system */,
                        int /* index */) const {}

      static std::string name() { return "AggregateForcing"; }

     private:
      std::vector<ST> s_;
    };

  }  // namespace forcing

}  // namespace elastica

// finally one with make_forcing

auto internal_first IGNORE_UNUSED = [](auto...) -> void {};
using ForcingFromMakeForcing = decltype(::elastica::forcing::make_forcing(
    ::elastica::forcing::make_force(internal_first),
    ::elastica::forcing::make_torque(internal_first))());

namespace {

  struct Checks {
    using CC = TestForcing<MockSystem>;

    template <typename T>
    struct CheckConformantForcing : std::true_type {
      static_assert(CC::conformant_forcing<T>(),
                    "A conformant forcing was recognized as non-conformant");
    };

    template <typename T>
    struct CheckNonConformantForcing : std::true_type {
      static_assert(CC::non_conformant_forcing<T>(),
                    "A non conformant forcing was recognized as conformant");
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

  using ForcingList =
      tmpl::list<::elastica::forcing::ForcingDefaulted<>,
                 ::elastica::forcing::ForcingReferenceCounted<>,
                 ::elastica::forcing::ForcingWithFundamentalTypes<>,
                 ::elastica::forcing::ForcingWithAggregateTypes<>,
                 ::elastica::forcing::ForcingWithNonCopyableTypes<>,
                 ForcingFromMakeForcing>;
  using NonForcingList = tmpl::list<int, float*, const char, volatile bool>;

  using conformant_forcing_checks IGNORE_UNUSED = decltype(
      tmpl::transform<ForcingList, Checks::CheckConformantForcing<tmpl::_1>>{});
  using non_conformant_forcing_checks IGNORE_UNUSED =
      decltype(tmpl::transform<NonForcingList,
                               Checks::CheckNonConformantForcing<tmpl::_1>>{});

  using IndexSetList =
      tmpl::list<decltype(::elastica::interface::at(1UL, 2UL)),
                 decltype(::elastica::interface::at(1UL, 5UL, 10UL, end - 2)),
                 decltype(::elastica::interface::range(1, 6, 1)),
                 decltype(::elastica::interface::range(1, end - 20, 2))>;
  using NonIndexSetList = tmpl::list<unsigned char, int, short, long>;

  using conformant_indexset_checks IGNORE_UNUSED =
      decltype(tmpl::transform<IndexSetList,
                               Checks::CheckConformantIndexSet<tmpl::_1>>{});
  using non_conformant_indexset_checks IGNORE_UNUSED =
      decltype(tmpl::transform<NonIndexSetList,
                               Checks::CheckNonConformantIndexSet<tmpl::_1>>{});

  using InterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<ForcingList, IndexSetList>{});
  using FirstNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<NonForcingList, IndexSetList>{});
  using SecondNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<ForcingList, NonIndexSetList>{});
  using ThirdNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<NonForcingList, NonIndexSetList>{});

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

  void test_forcing_with_default() {
    using ConcreteForcing = TestForcing<MockSystem>;

    MockSystem f{100};

    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");
      ::elastica::forcing::ForcingDefaulted<> forcing{};
      CHECK(cf.is_not_valid());
      cf.with(std::move(forcing), ::elastica::interface::at(1UL));
      CHECK(f.n_indices == 100);
      CHECK(not cf.is_not_valid());
    }

    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");
      ::elastica::forcing::ForcingDefaulted<> forcing{};
      auto x = ::elastica::interface::at(1UL, end - 2);
      CHECK(cf.is_not_valid());
      cf.with(std::move(forcing), x);
      CHECK(f.n_indices == 100);
      CHECK(not cf.is_not_valid());
    }

    {  // xvalue
      ConcreteForcing cf(6UL, f, "test");
      cf.with(::elastica::forcing::ForcingDefaulted<>{},
              ::elastica::interface::at(2UL));
      CHECK(f.n_indices == 100);
      CHECK(not cf.is_not_valid());
    }
  }

  void test_forcing_with_object_counts() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // prvalue
      {
        ConcreteForcing cf(6UL, f, "test");
        CHECK(cf.is_not_valid());
        cf.with(::elastica::forcing::ForcingReferenceCounted<>{},
                ::elastica::interface::at(2UL));
        CHECK(f.n_indices == 100);
        CHECK(not cf.is_not_valid());
        // While the contents are moved around, three empty forcing are
        // created and destroyed, 1 more forcing is within

        CHECK(counter<TestForcingObjectCount>::objects_alive ==
              1);  // 1 object is alive within cc
        CHECK(counter<TestForcingObjectCount>::objects_created ==
              4);  // 4 objects in total
        CHECK(counter<TestForcingObjectCount>::objects_copied ==
              0);  // ensure no copies
        CHECK(counter<TestForcingObjectCount>::objects_moved == 3);  // 4 -1 ?
      }

      CHECK(counter<TestForcingObjectCount>::objects_alive ==
            0);  // After cf.dies, the object should too!
    }

    {  // xvalue
      {
        ConcreteForcing cf(6UL, f, "test");
        ::elastica::forcing::ForcingReferenceCounted<> forcing{};
        CHECK(cf.is_not_valid());
        cf.with(std::move(forcing), ::elastica::interface::at(1UL));
        CHECK(f.n_indices == 100);
        CHECK(not cf.is_not_valid());
        // While the contents are moved around, three empty forcing are
        // created and destroyed, 1 more forcing is within

        CHECK(counter<TestForcingObjectCount>::objects_alive ==
              2);  // 1 object is alive within cf. 1 is in the current scope
        CHECK(counter<TestForcingObjectCount>::objects_created ==
              4 + 4);  // 4 objects in total, adding on to the previous 4
        CHECK(counter<TestForcingObjectCount>::objects_copied ==
              0);  // ensure no copies
        CHECK(counter<TestForcingObjectCount>::objects_moved ==
              3 + 3);  // 4 -1 ?
      }

      CHECK(counter<TestForcingObjectCount>::objects_alive ==
            0);  // After cf.dies, the object should too!
    }
  }

  void test_forcing_with_fundamental_types() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      ::elastica::forcing::ForcingWithFundamentalTypes<> forcing{1, 2.0F, 'c',
                                                                 i, &j};
      CHECK(f.n_indices == 100);
      cf.with(std::move(forcing), ::elastica::interface::at(1UL, 4UL, 3UL));
      CHECK(not cf.is_not_valid());
    }

    {  // xvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      cf.with(::elastica::forcing::ForcingWithFundamentalTypes<>{1, 2.0F, 'c',
                                                                 i, &j},
              ::elastica::interface::at(3UL, 2UL, 1UL));
      CHECK(f.n_indices == 100);
      CHECK(not cf.is_not_valid());
    }
  }

  void test_forcing_with_aggregate_types() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      ::elastica::forcing::ForcingWithAggregateTypes<> forcing{"ODIs", "are",
                                                               "good"};
      CHECK(f.n_indices == 100);
      cf.with(std::move(forcing), ::elastica::interface::at(1UL, 4UL, 3UL));
      CHECK(not cf.is_not_valid());
    }

    {  // xvalue
      ConcreteForcing cf(6UL, f, "test");
      cf.with(::elastica::forcing::ForcingWithAggregateTypes<>{"Tests", "are",
                                                               "better"},
              ::elastica::interface::at(3UL, 2UL, 1UL));
      CHECK(f.n_indices == 100);
      CHECK(not cf.is_not_valid());
    }
  }

  void test_forcing_with_ranged() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      ::elastica::forcing::ForcingWithFundamentalTypes<> forcing{1, 2.0F, 'c',
                                                                 i, &j};
      CHECK(f.n_indices == 100);
      cf.with(std::move(forcing),
              ::elastica::interface::at(
                  ::elastica::interface::range(1UL, end - 20, 2UL)));
      CHECK(not cf.is_not_valid());
    }

    {  // xvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      cf.with(::elastica::forcing::ForcingWithFundamentalTypes<>{1, 2.0F, 'c',
                                                                 i, &j},
              ::elastica::interface::at(
                  ::elastica::interface::range(15UL, 25UL, 2)));
      CHECK(f.n_indices == 100);
      CHECK(not cf.is_not_valid());
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.Forcing.record",
                     "[Unit][Simulator][Interface][Forcing]") {
    using RegistrarList = tmpl::list<char>;
    using ForcingInterface = TestForcingImpl<double, RegistrarList>;
    double d{5.12312};
    {
      ForcingInterface cc(8UL, d, "testing");
      cc.record_name_of<int>();
      CHECK(cc.concrete_forcing_type() == "int");
      cc.record_name_of<float>();
      CHECK(cc.concrete_forcing_type() == "float");
      cc.record_name_of<::elastica::forcing::ForcingDefaulted<>>();
      CHECK(cc.concrete_forcing_type() == "ForcingDefaulted");
    }

    // test the with interface for recording
    {
      ForcingInterface cc(8UL, d, "testing");
      ::elastica::forcing::ForcingDefaulted<> forcing{};
      cc.with(std::move(forcing));
      CHECK(cc.concrete_forcing_type() == "ForcingDefaulted");
    }
    {
      ForcingInterface cc(8UL, d, "testing");
      cc.with<::elastica::forcing::ForcingDefaulted<>>();
      CHECK(cc.concrete_forcing_type() == "ForcingDefaulted");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.Forcing.set_name",
                     "[Unit][Simulator][Interface][Forcing]") {
    using RegistrarList = tmpl::list<char>;
    using ForcingInterface = TestForcingImpl<double, RegistrarList>;
    double d{5.12312};
    {
      ForcingInterface cc(8UL, d, "testing");
      CHECK(cc.id().name == "testing");
      cc.set_name("boo!");
      CHECK(cc.id().name == "boo!");
    }
    {
      ForcingInterface cc(8UL, d, "testing");
      CHECK(cc.id().name == "testing");
      // cc.with_name("boo!");
      // CHECK(cc.id().name == "boo!");
    }
    {
      CHECK(ForcingInterface(8UL, d, "testing")
                .set_name("boo!")
                .id()
                .name == "boo!");
    }
    {
      CHECK(ForcingInterface(8UL, d, "testing")
                // .with_name("boo!")
                .id()
                .name == "testing");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.Forcing.with",
                     "[Unit][Simulator][Interface][Forcing]") {
    test_forcing_with_default();
    test_forcing_with_object_counts();
    test_forcing_with_fundamental_types();
    test_forcing_with_aggregate_types();
    test_forcing_with_ranged();
  }

  void test_forcing_with_fundamental_types_via_forwarding_with() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      ::elastica::forcing::ForcingWithFundamentalTypes<> forcing{1, 2.0F, 'c',
                                                                 i, &j};
      CHECK(f.n_indices == 100);
      auto x = cf.with(std::move(forcing));
      // shouldnt be valid in this intermediate state
      CHECK(cf.is_not_valid());
      // but has a valid name
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
      x.at(1UL, 2UL, 3UL, 4UL);
      CHECK(not cf.is_not_valid());
      CHECK(f.n_indices == 100);
    }

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      ::elastica::forcing::ForcingWithFundamentalTypes<> forcing{1, 2.0F, 'c',
                                                                 i, &j};
      // intended usage
      cf.with(std::move(forcing)).at(1UL, 2UL, 3UL, 4UL);
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
      CHECK(f.n_indices == 100);
    }

    {  // xvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      cf.with(::elastica::forcing::ForcingWithFundamentalTypes<>{1, 2.0F, 'c',
                                                                 i, &j})
          .at(1UL, 2UL, 3UL);
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
    }
  }

  void test_forcing_with_aggregate_types_via_forwarding_with() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      ::elastica::forcing::ForcingWithAggregateTypes<> forcing{"ODIs", "are",
                                                               "good"};
      cf.with(std::move(forcing)).at(1UL, 4UL, 3UL);
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithAggregateTypes");
      CHECK(f.n_indices == 100);
    }

    {  // xvalue
      ConcreteForcing cf(6UL, f, "test");
      cf.with(::elastica::forcing::ForcingWithAggregateTypes<>{"Tests", "are",
                                                               "better"})
          .at(3UL, 2UL, 1UL);
      CHECK(cf.concrete_forcing_type() == "ForcingWithAggregateTypes");
      CHECK(f.n_indices == 100);
      CHECK(not cf.is_not_valid());
    }
  }

  void test_forcing_with_fundamental_types_via_forwarding_with_ranged() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      ::elastica::forcing::ForcingWithFundamentalTypes<> forcing{1, 2.0F, 'c',
                                                                 i, &j};
      CHECK(f.n_indices == 100);
      auto x = cf.with(std::move(forcing));
      // shouldnt be valid in this intermediate state
      CHECK(cf.is_not_valid());
      // but has a valid name
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
      x.at(::elastica::interface::range(15UL, 25UL, 2));
      CHECK(not cf.is_not_valid());
      CHECK(f.n_indices == 100);
    }

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      ::elastica::forcing::ForcingWithFundamentalTypes<> forcing{1, 2.0F, 'c',
                                                                 i, &j};
      // intended usage
      cf.with(std::move(forcing))
          .at(::elastica::interface::range(15UL, ::elastica::end - 5, 5));
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
      CHECK(f.n_indices == 100);
    }

    {  // xvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      cf.with(::elastica::forcing::ForcingWithFundamentalTypes<>{1, 2.0F, 'c',
                                                                 i, &j})
          .at(::elastica::interface::range(15UL, ::elastica::end - 20));
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Forcing.Forcing.forwarding_with",
                     "[Unit][Simulator][Interface][Forcing]") {
    test_forcing_with_fundamental_types_via_forwarding_with();
    test_forcing_with_aggregate_types_via_forwarding_with();
    test_forcing_with_fundamental_types_via_forwarding_with_ranged();
  }

  void test_forcing_with_fundamental_types_via_forwarded_arguments() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      CHECK(f.n_indices == 100);
      auto x = cf.with<::elastica::forcing::ForcingWithFundamentalTypes<>>(
          1, 2.0F, 'c', i, &j);
      // shouldnt be valid in this intermediate state
      CHECK(cf.is_not_valid());
      // but has a valid name
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
      x.at(1UL, 2UL, 3UL, 4UL);
      CHECK(not cf.is_not_valid());
      CHECK(f.n_indices == 100);
    }

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      // intended usage
      cf.with<::elastica::forcing::ForcingWithFundamentalTypes<>>(1, 2.0F, 'c',
                                                                  i, &j)
          .at(1UL, 2UL, 3UL, 4UL);
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
      CHECK(f.n_indices == 100);
    }
  }

  void test_forcing_with_aggregate_types_via_forwarded_arguments() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      cf.with<::elastica::forcing::ForcingWithAggregateTypes<>>("ODIs", "are",
                                                                "good")
          .at(1UL, 4UL, 3UL);
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithAggregateTypes");
      CHECK(f.n_indices == 100);
    }

    {  // xvalue
      ConcreteForcing cf(6UL, f, "test");
      auto x = cf.with<::elastica::forcing::ForcingWithAggregateTypes<>>(
          "Tests", "are", "better");
      CHECK(cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithAggregateTypes");
      x.at(3UL, 2UL, 1UL);
      CHECK(f.n_indices == 100);
      CHECK(not cf.is_not_valid());
    }
  }

  void test_forcing_with_noncopyable_types_via_forwarded_arguments() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      // float r(2.0F), p(5.000F), q(8.000F);
      cf.with<::elastica::forcing::ForcingWithNonCopyableTypes<>>(
            std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
            std::make_unique<float>(80.0F))
          .at(::elastica::interface::range(15UL, ::elastica::end - 20));
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithNonCopyableTypes");
      CHECK(f.n_indices == 100);
    }
  }

  void test_forcing_with_fundamental_types_via_forwarded_arguments_ranged() {
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      CHECK(f.n_indices == 100);
      auto x = cf.with<::elastica::forcing::ForcingWithFundamentalTypes<>>(
          1, 2.0F, 'c', i, &j);
      // shouldnt be valid in this intermediate state
      CHECK(cf.is_not_valid());
      // but has a valid name
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
      x.at(::elastica::interface::range(15UL, ::elastica::end - 20, 2));
      CHECK(not cf.is_not_valid());
      CHECK(f.n_indices == 100);
    }

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      // intended usage
      cf.with<::elastica::forcing::ForcingWithFundamentalTypes<>>(1, 2.0F, 'c',
                                                                  i, &j)
          .at(::elastica::interface::range(15UL, ::elastica::end - 20, 5));
      CHECK(not cf.is_not_valid());
      CHECK(cf.concrete_forcing_type() == "ForcingWithFundamentalTypes");
      CHECK(f.n_indices == 100);
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Forcing.with_forwarded_arguments",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    test_forcing_with_fundamental_types_via_forwarded_arguments();
    test_forcing_with_aggregate_types_via_forwarded_arguments();
    test_forcing_with_noncopyable_types_via_forwarded_arguments();
    test_forcing_with_fundamental_types_via_forwarded_arguments_ranged();
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Forcing.Forcing.exception_for_incorrect_indices",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");

      //      cf.with<::elastica::forcing::ForcingWithNonCopyableTypes<>>(
      //            std::make_unique<float>(2.0F),
      //            std::make_unique<float>(5.0F),
      //            std::make_unique<float>(80.0F))
      //          .at(1, 4, 43);
      //
      CHECK_THROWS_NESTED_WITH(
          cf.unsafe_with(
              ::elastica::forcing::ForcingWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(1UL, 4UL, 143UL, end - 150)),
          "Attempts to force system", "Number of degrees of freedom exceeded");

      ConcreteForcing new_cf(6UL, f, "test");

      CHECK_THROWS_NESTED_WITH(
          new_cf.unsafe_with<>(
              ::elastica::forcing::ForcingWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(1UL, 4UL, 143UL, end - 150)),
          "Attempts to force system", "Number of degrees of freedom exceeded");
    }

    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");

      CHECK_THROWS_NESTED_WITH(
          cf.unsafe_with(
              ::elastica::forcing::ForcingWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(
                  ::elastica::interface::range(end - 200, 50UL, 1))),
          "Attempts to force system", "Number of degrees of freedom exceeded");

      ConcreteForcing new_cf(6UL, f, "test");

      CHECK_THROWS_NESTED_WITH(
          new_cf.unsafe_with(
              ::elastica::forcing::ForcingWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(
                  ::elastica::interface::range(end - 200, 50UL, 1))),
          "Attempts to force system", "Number of degrees of freedom exceeded");
    }

    {  // rvalue
      ConcreteForcing cf(6UL, f, "test");

      CHECK_THROWS_NESTED_WITH(
          cf.unsafe_with(
              ::elastica::forcing::ForcingWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(
                  ::elastica::interface::range(40UL, 200UL, 1))),
          "Attempts to force system", "Number of degrees of freedom exceeded");

      ConcreteForcing new_cf(6UL, f, "test");

      CHECK_THROWS_NESTED_WITH(
          new_cf.unsafe_with(
              ::elastica::forcing::ForcingWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::at(
                  ::elastica::interface::range(40UL, 200UL, 1))),
          "Attempts to force system", "Number of degrees of freedom exceeded");
    }
  }

  template <template <typename> class ForcingType, typename... Args>
  std::unique_ptr<::elastica::forcing::detail::ForcingBase<int>> make_forcing(
      Args&&... args) {
    return std::make_unique<ForcingType<int>>(std::forward<Args>(args)...);
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
      "Unit.Simulator.Interface.Forcing.Forcing.with_unique_ptr",
      "[Unit][Simulator][Interface][Forcing]") {
    // clang-format on
    using ConcreteForcing = TestForcing<MockSystem>;
    MockSystem f{100};
    {  // rvalue
      int i(2);
      int j(4);
      ConcreteForcing cf(6UL, f, "test");
      CHECK(cf.is_not_valid());
      // registar type doesnt really matter here
      //      std::unique_ptr<::elastica::forcing::detail::ForcingBase<int>>
      //          runtime_forcing = std::make_unique<
      //              ::elastica::forcing::ForcingWithFundamentalTypes<int>>(
      //              1, 2.0F, 'c', i, &j);
      auto runtime_forcing =
          make_forcing<::elastica::forcing::ForcingWithFundamentalTypes>(
              1, 2.0F, 'c', i, &j);
      CHECK(f.n_indices == 100);
      cf.with(std::move(runtime_forcing), make_at(1UL, 4UL, 3UL));
      CHECK(not cf.is_not_valid());
    }
  }

}  // namespace
