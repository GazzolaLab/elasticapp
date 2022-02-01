//
// Created by Tejaswin Parthasarathy on 1/28/21.
//

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestNestedExceptions.hpp"
#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Connections/Connection.hpp"
#include "Simulator/Interface/Connections/ConnectionBase.hpp"
#include "Simulator/Interface/Indexers/Pair.hpp"
#include "Utilities/Defer.hpp"
#include "Utilities/End.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

// make a constraint
#include "Simulator/Connections/Connection.hpp"
#include "Simulator/Connections/MakeConnection.hpp"
#include "Simulator/Connections/Registration.hpp"
#include "Simulator/Interface/Connections/MockConnects.hpp"
#include "Utilities/Registration.hpp"

/// mocks
#include "Simulator/Interface/MockSystems.hpp"

namespace {

  using namespace ::elastica;
  using namespace ::elastica::interface::connections;

  using ::mock::MockSystem;
  using ::mock::TestConnection;
  using ::mock::TestConnectionImpl;

  namespace {

    using RegistrarListCheck = tmpl::list<int, float, char>;
    using FirstSystemType = float;
    using SecondSystemType = int;
    static_assert(std::is_base_of<ConnectionBase<RegistrarListCheck>,
                                  Connection<FirstSystemType, SecondSystemType,
                                             RegistrarListCheck>>::value,
                  "Base violation!");

  }  // namespace

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Connection.constructor",
      "[Unit][Simulator][Interface][Connections]") {
    {
      using RegistrarList = tmpl::list<>;
      using ConcreteConnection =
          TestConnectionImpl<std::reference_wrapper<int>,
                             std::reference_wrapper<float>, RegistrarList>;
      int i(2);
      float f(2.0F);
      std::size_t second_index(6UL);
      ConcreteConnection cc(second_index, i, f, "test");

      CHECK(cc.first_concrete_system_type() == "int");
      CHECK(cc.second_concrete_system_type() == "float");
    }

    {
      using RegistrarList = tmpl::list<int, float, char>;
      using ConcreteConnection =
          TestConnectionImpl<std::reference_wrapper<float>,
                             std::reference_wrapper<char>, RegistrarList>;
      float f(4.0F);
      char c('a');
      ConcreteConnection cc(6UL, f, c, "test");

      CHECK(cc.first_concrete_system_type() == "float");
      CHECK(cc.second_concrete_system_type() == "char");
    }

    {
      using RegistrarList = tmpl::list<char>;
      using ConcreteConnection =
          TestConnectionImpl<std::reference_wrapper<double>,
                             std::reference_wrapper<bool>, RegistrarList>;
      double d{5.12312};
      bool b{true};
      ConcreteConnection cc(8UL, d, b, "testing");

      CHECK(cc.first_concrete_system_type() == "double");
      CHECK(cc.second_concrete_system_type() == "bool");
    }
  }

}  // namespace

namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionDefaulted) {
     public:
      ConnectionDefaulted() = default;
      ConnectionDefaulted(ConnectionDefaulted &&) noexcept {};
      // move constructor is deleted if a destructor is defined
      //      ~ConnectionDefaulted(){
      //        std::cout << "Destructor caleld!" << std::endl;
      //      }

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /* first_system */,
                         int /* first_index */,
                         SecondSystem& /* second_system */,
                         int /* second_index */) const {}

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /* first_system */,
                        int /* first_index */,
                        SecondSystem& /* second_system */,
                        int /* second_index */) const {}

      static std::string name() { return "ConnectionDefault"; }
    };

  }  // namespace connections

}  // namespace elastica

struct TestConnectionObjectCount : counter<TestConnectionObjectCount> {
  /*  ReferenceCount() = default;
    ReferenceCount(ReferenceCount const& other) : count(other.count + 1) {}
    ReferenceCount(ReferenceCount&& other) noexcept : count(other.count + 1) {}
    unsigned int count = 0U;*/
};

namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionReferenceCounted) {
     public:
      explicit ConnectionReferenceCounted() : r(){};
      ConnectionReferenceCounted(ConnectionReferenceCounted && other) noexcept
          : r(std::move(other.r)){};
      // move constructor is deleted if a destructor is defined
      ~ConnectionReferenceCounted() override = default;
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /* first_system */,
                         int /* first_index */,
                         SecondSystem& /* second_system */,
                         int /* second_index */) const {}

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /* first_system */,
                        int /* first_index */,
                        SecondSystem& /* second_system */,
                        int /* second_index */) const {}

      static std::string name() { return "ConnectionRefCounted"; }

      TestConnectionObjectCount r;
    };

  }  // namespace connections

}  // namespace elastica

namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithFundamentalTypes) {
     public:
      ConnectionWithFundamentalTypes(int a, float b, char c, int& d, int* p)
          : a_(a), b_(b), c_(c), d_(d), p_(p){};
      ConnectionWithFundamentalTypes(ConnectionWithFundamentalTypes &&
                                     other) noexcept
          : a_(other.a_),
            b_(other.b_),
            c_(other.c_),
            d_(other.d_),
            p_(other.p_) {}
      ~ConnectionWithFundamentalTypes() override = default;

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /* first_system */,
                         int /* first_index */,
                         SecondSystem& /* second_system */,
                         int /* second_index */) const {}

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /* first_system */,
                        int /* first_index */,
                        SecondSystem& /* second_system */,
                        int /* second_index */) const {}

      static std::string name() noexcept { return "FundamentalConnection"; }

     private:
      int a_;
      float b_;
      char c_;
      int& d_;
      int* p_;
    };

  }  // namespace connections

}  // namespace elastica

namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithAggregateTypes) {
     public:
      using ST = std::string;
      ConnectionWithAggregateTypes(ST s1, ST s2, ST s3)
          : s_{std::move(s1), std::move(s2), std::move(s3)} {};
      ConnectionWithAggregateTypes(ConnectionWithAggregateTypes &&
                                   other) noexcept
          : s_(std::move(other.s_)) {}
      ~ConnectionWithAggregateTypes() override = default;

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /* first_system */,
                         int /* first_index */,
                         SecondSystem& /* second_system */,
                         int /* second_index */) const {}

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /* first_system */,
                        int /* first_index */,
                        SecondSystem& /* second_system */,
                        int /* second_index */) const {}

      static std::string name() { return "AggregateConnection"; }

     private:
      std::vector<ST> s_;
    };

  }  // namespace connections

}  // namespace elastica

namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(ConnectionWithNonCopyableTypes) {
     public:
      using ST = std::unique_ptr<float>;
      ConnectionWithNonCopyableTypes(ST && s1, ST && s2, ST && s3) {
        s_.emplace_back(std::move(s1));
        s_.emplace_back(std::move(s2));
        s_.emplace_back(std::move(s3));
      }
      ConnectionWithNonCopyableTypes(ConnectionWithNonCopyableTypes &&
                                     other) noexcept
          : s_(std::move(other.s_)) {}
      ~ConnectionWithNonCopyableTypes() override = default;

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /* first_system */,
                         int /* first_index */,
                         SecondSystem& /* second_system */,
                         int /* second_index */) const {}

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /* first_system */,
                        int /* first_index */,
                        SecondSystem& /* second_system */,
                        int /* second_index */) const {}

      static std::string name() { return "AggregateConnection"; }

     private:
      std::vector<ST> s_;
    };

  }  // namespace connections

}  // namespace elastica

// deferred
namespace elastica {

  namespace connections {

    ELASTICA_DEFINE_CONNECTION(DeferredConnectionWithAggregateTypes) {
     public:
      using ST = std::string;
      DeferredConnectionWithAggregateTypes(ST s1, ST s2, ST s3)
          : s_{std::move(s1), std::move(s2), std::move(s3)} {};
      DeferredConnectionWithAggregateTypes(
          DeferredConnectionWithAggregateTypes && other) noexcept
          : s_(std::move(other.s_)) {}
      ~DeferredConnectionWithAggregateTypes() override = default;

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(
          TimeType /* time */, FirstSystem& /* first_system */, SecondSystem &
          /* second_system */) const {}

      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /* first_system */,
                        SecondSystem & /* second_system */
      ) const {}

      static std::string name() { return "AggregateConnection"; }

     private:
      std::vector<ST> s_;
    };

  }  // namespace connections

}  // namespace elastica

// finally one with make_connection

auto internal_connection_first IGNORE_UNUSED = [](auto...) -> void {};
using ConnectionFromMakeConnection =
    decltype(::elastica::connections::make_connection(
        ::elastica::connections::make_force(internal_connection_first),
        ::elastica::connections::make_torque(internal_connection_first))());

namespace {

  struct Checks {
    using CC = TestConnection<std::reference_wrapper<MockSystem>,
                              std::reference_wrapper<MockSystem>>;

    template <typename T>
    struct CheckConformantConnection : std::true_type {
      static_assert(CC::conformant_connection<T>(),
                    "A conformant connection was recognized as non-conformant");
    };

    template <typename T>
    struct CheckNonConformantConnection : std::true_type {
      static_assert(CC::non_conformant_connection<T>(),
                    "A non conformant connection was recognized as conformant");
    };

    template <typename T>
    struct CheckConformantIndexPair : std::true_type {
      static_assert(CC::conformant_index_pair<T>(),
                    "A conformant index pair was recognized as non-conformant");
    };

    template <typename T>
    struct CheckNonConformantIndexPair : std::true_type {
      static_assert(CC::non_conformant_index_pair<T>(),
                    "A non conformant index pair was recognized as conformant");
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

  using ConnectionsList =
      tmpl::list<::elastica::connections::ConnectionDefaulted<>,
                 ::elastica::connections::ConnectionReferenceCounted<>,
                 ::elastica::connections::ConnectionWithFundamentalTypes<>,
                 ::elastica::connections::ConnectionWithAggregateTypes<>,
                 ::elastica::connections::ConnectionWithNonCopyableTypes<>,
                 ConnectionFromMakeConnection>;
  using NonConnectionsList = tmpl::list<int, float*, const char, volatile bool>;

  using conformant_connections_checks IGNORE_UNUSED =
      decltype(tmpl::transform<ConnectionsList,
                               Checks::CheckConformantConnection<tmpl::_1>>{});
  using non_conformant_connections_checks IGNORE_UNUSED = decltype(
      tmpl::transform<NonConnectionsList,
                      Checks::CheckNonConformantConnection<tmpl::_1>>{});

  using IndexPairList =
      tmpl::list<decltype(::elastica::interface::pair(1UL, 2UL)),
                 decltype(::elastica::interface::pair(end - 2, end - 2)),
                 decltype(::elastica::interface::pair(10UL, end - 2)),
                 decltype(::elastica::interface::pair(defer, defer))>;
  using NonIndexPairList =
      tmpl::list<unsigned char, int, short, long,
                 decltype(::elastica::interface::at(1UL, 6UL, 12UL, end - 3,
                                                    2UL)),
                 decltype(::elastica::interface::range(1UL, 6UL, 1))>;

  using conformant_indexpair_checks IGNORE_UNUSED =
      decltype(tmpl::transform<IndexPairList,
                               Checks::CheckConformantIndexPair<tmpl::_1>>{});
  using non_conformant_indexpair_checks IGNORE_UNUSED = decltype(
      tmpl::transform<NonIndexPairList,
                      Checks::CheckNonConformantIndexPair<tmpl::_1>>{});

  using InterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<ConnectionsList, IndexPairList>{});
  using FirstNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<NonConnectionsList, IndexPairList>{});
  using SecondNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<ConnectionsList, NonIndexPairList>{});
  using ThirdNonInterfaceList IGNORE_UNUSED =
      decltype(tmpl::cartesian_product<NonConnectionsList, NonIndexPairList>{});

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

  void test_connection_with_default() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;

    MockSystem f{100};
    MockSystem g{50};

    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      ::elastica::connections::ConnectionDefaulted<> connection{};
      CHECK(cc.is_not_valid());
      cc.with(std::move(connection),
              ::elastica::interface::pair(end - 2, 10UL));
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }

    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      ::elastica::connections::ConnectionDefaulted<> connection{};
      auto x = ::elastica::interface::pair(1UL, end - 2);
      CHECK(cc.is_not_valid());
      cc.with(std::move(connection), x);
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }

    {  // xvalue
      ConcreteConnection cc(6UL, f, g, "test");
      cc.with(::elastica::connections::ConnectionDefaulted<>{},
              ::elastica::interface::pair(end - 5, end - 6));
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }
  }

  void test_connection_with_object_counts() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // prvalue
      {
        ConcreteConnection cc(6UL, f, g, "test");
        CHECK(cc.is_not_valid());
        cc.with(::elastica::connections::ConnectionReferenceCounted<>{},
                ::elastica::interface::pair(2UL, end - 5));
        CHECK(f.n_indices == 100);
        CHECK(g.n_indices == 50);
        CHECK(not cc.is_not_valid());
        // While the contents are moved around, three empty connections are
        // created and destroyed, 1 more connection is within

        CHECK(counter<TestConnectionObjectCount>::objects_alive ==
              1);  // 1 object is alive within cc
        CHECK(counter<TestConnectionObjectCount>::objects_created ==
              4);  // 4 objects in total
        CHECK(counter<TestConnectionObjectCount>::objects_copied ==
              0);  // ensure no copies
        CHECK(counter<TestConnectionObjectCount>::objects_moved ==
              3);  // 4 -1 ?
      }

      CHECK(counter<TestConnectionObjectCount>::objects_alive ==
            0);  // After cc dies, the object should too!
    }

    {  // xvalue
      {
        ConcreteConnection cc(6UL, f, g, "test");
        ::elastica::connections::ConnectionReferenceCounted<> connection{};
        CHECK(cc.is_not_valid());
        cc.with(std::move(connection), ::elastica::interface::pair(1UL, 5UL));
        CHECK(f.n_indices == 100);
        CHECK(g.n_indices == 50);
        CHECK(not cc.is_not_valid());
        // While the contents are moved around, three empty connections are
        // created and destroyed, 1 more connection is within

        CHECK(counter<TestConnectionObjectCount>::objects_alive ==
              2);  // 1 object is alive within cc, 1 is in the current scope
        CHECK(counter<TestConnectionObjectCount>::objects_created ==
              4 + 4);  // 4 objects in total, adding on to the previous 4
        CHECK(counter<TestConnectionObjectCount>::objects_copied ==
              0);  // ensure no copies
        CHECK(counter<TestConnectionObjectCount>::objects_moved ==
              3 + 3);  // 4 -1 ?
      }

      CHECK(counter<TestConnectionObjectCount>::objects_alive ==
            0);  // After cc dies, the object should too!
    }
  }

  void test_connection_with_fundamental_types() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      ::elastica::connections::ConnectionWithFundamentalTypes<> connection{
          1, 2.0F, 'c', i, &j};
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      cc.with(std::move(connection),
              ::elastica::interface::pair(end - 3, end - 5));
      CHECK(not cc.is_not_valid());
    }

    {  // xvalue
      int i(2);
      int j(4);
      ConcreteConnection cc(6UL, f, g, "test");
      cc.with(
          ::elastica::connections::ConnectionWithFundamentalTypes<>{1, 2.0F,
                                                                    'c', i, &j},
          ::elastica::interface::pair(3UL, 2UL));
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }
  }

  void test_connection_with_aggregate_types() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      ::elastica::connections::ConnectionWithAggregateTypes<> connection{
          "ODIs", "are", "good"};
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      cc.with(std::move(connection),
              ::elastica::interface::pair(1UL, end - 20));
      CHECK(not cc.is_not_valid());
    }

    {  // xvalue
      ConcreteConnection cc(6UL, f, g, "test");
      cc.with(
          ::elastica::connections::ConnectionWithAggregateTypes<>{
              "Tests", "are", "better"},
          ::elastica::interface::pair(30UL, 1UL));
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }
  }

  void test_deferred_connection_with_aggregate_types() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      ::elastica::connections::DeferredConnectionWithAggregateTypes<>
          connection{"ODIs", "are", "good"};
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      cc.with(std::move(connection), ::elastica::interface::pair(defer, defer));
      CHECK(not cc.is_not_valid());
    }

    {  // xvalue
      ConcreteConnection cc(6UL, f, g, "test");
      cc.with(
          ::elastica::connections::DeferredConnectionWithAggregateTypes<>{
              "Tests", "are", "better"},
          ::elastica::interface::pair(defer, defer));
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.Connection.record",
                     "[Unit][Simulator][Interface][Connections]") {
    using RegistrarList = tmpl::list<char>;
    using ConnectionInterface =
        TestConnectionImpl<std::reference_wrapper<double>,
                           std::reference_wrapper<bool>, RegistrarList>;
    double d{5.12312};
    bool b{true};
    {
      ConnectionInterface cc(8UL, d, b, "testing");
      cc.record_name_of<int>();
      CHECK(cc.concrete_connection_type() == "int");
      cc.record_name_of<float>();
      CHECK(cc.concrete_connection_type() == "float");
      cc.record_name_of<::elastica::connections::ConnectionDefaulted<>>();
      CHECK(cc.concrete_connection_type() == "ConnectionDefaulted");
    }

    // test the with interface for recording
    {
      ConnectionInterface cc(8UL, d, b, "testing");
      ::elastica::connections::ConnectionDefaulted<> connection{};
      cc.with(std::move(connection));
      CHECK(cc.concrete_connection_type() == "ConnectionDefaulted");
    }
    {
      ConnectionInterface cc(8UL, d, b, "testing");
      cc.with<::elastica::connections::ConnectionDefaulted<>>();
      CHECK(cc.concrete_connection_type() == "ConnectionDefaulted");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.Connection.set_name",
                     "[Unit][Simulator][Interface][Connections]") {
    using RegistrarList = tmpl::list<char>;
    using ConnectionInterface =
        TestConnectionImpl<std::reference_wrapper<double>,
                           std::reference_wrapper<bool>, RegistrarList>;
    double d{5.12312};
    bool b{true};
    {
      ConnectionInterface cc(8UL, d, b, "testing");
      CHECK(cc.id().name == "testing");
      cc.set_name("boo!");
      CHECK(cc.id().name == "boo!");
    }
    {
      ConnectionInterface cc(8UL, d, b, "testing");
      CHECK(cc.id().name == "testing");
      cc.with_name("boo!");
      CHECK(cc.id().name == "boo!");
    }
    {
      CHECK(ConnectionInterface(8UL, d, b, "testing")
                .set_name("boo!")
                .id()
                .name == "boo!");
    }
    {
      CHECK(ConnectionInterface(8UL, d, b, "testing")
                .with_name("boo!")
                .id()
                .name == "boo!");
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Connections.Connection.with",
                     "[Unit][Simulator][Interface][Connections]") {
    test_connection_with_default();
    test_connection_with_object_counts();
    test_connection_with_fundamental_types();
    test_connection_with_aggregate_types();
    test_deferred_connection_with_aggregate_types();
  }

  void test_connection_with_fundamental_types_via_forwarding_with() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      ::elastica::connections::ConnectionWithFundamentalTypes<> connection{
          1, 2.0F, 'c', i, &j};
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      auto x = cc.with(std::move(connection));
      // shouldnt be valid in this intermediate state
      CHECK(cc.is_not_valid());
      // but has a valid name
      CHECK(cc.concrete_connection_type() == "ConnectionWithFundamentalTypes");
      x.at(::elastica::interface::pair(30UL, 1UL));
      CHECK(not cc.is_not_valid());
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      ::elastica::connections::ConnectionWithFundamentalTypes<> connection{
          1, 2.0F, 'c', i, &j};
      // intended usage
      cc.with(std::move(connection))
          .at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_connection_type() == "ConnectionWithFundamentalTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }

    {  // xvalue
      int i(2);
      int j(4);
      ConcreteConnection cc(6UL, f, g, "test");
      cc.with(::elastica::connections::ConnectionWithFundamentalTypes<>{
                  1, 2.0F, 'c', i, &j})
          .at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_connection_type() == "ConnectionWithFundamentalTypes");
    }
  }

  void test_connection_with_aggregate_types_via_forwarding_with() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      ::elastica::connections::ConnectionWithAggregateTypes<> connection{
          "ODIs", "are", "good"};
      cc.with(std::move(connection))
          .at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_connection_type() == "ConnectionWithAggregateTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }

    {  // xvalue
      ConcreteConnection cc(6UL, f, g, "test");
      cc.with(::elastica::connections::ConnectionWithAggregateTypes<>{
                  "Tests", "are", "better"})
          .at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(cc.concrete_connection_type() == "ConnectionWithAggregateTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }
  }

  void test_deferred_connection_with_aggregate_types_via_forwarding_with() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      ::elastica::connections::DeferredConnectionWithAggregateTypes<>
          connection{"ODIs", "are", "good"};
      cc.with(std::move(connection))
          .at(::elastica::interface::pair(defer, defer));
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_connection_type() ==
            "DeferredConnectionWithAggregateTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }

    {  // xvalue
      ConcreteConnection cc(6UL, f, g, "test");
      /// [dce]
      // ::elastica::connections::DeferredConnectionWithAggregateTypes<> only
      // has deferred methods
      cc.with(elastica::connections::DeferredConnectionWithAggregateTypes<>{
                  "Tests", "are", "better"})
          .at(elastica::interface::pair(elastica::defer, elastica::defer));
      /// [dce]

      CHECK(cc.concrete_connection_type() ==
            "DeferredConnectionWithAggregateTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Connection.forwarding_with",
      "[Unit][Simulator][Interface][Connections]") {
    test_connection_with_fundamental_types_via_forwarding_with();
    test_connection_with_aggregate_types_via_forwarding_with();
    test_deferred_connection_with_aggregate_types_via_forwarding_with();
  }

  void test_connection_with_fundamental_types_via_forwarded_arguments() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      auto x =
          cc.with<::elastica::connections::ConnectionWithFundamentalTypes<>>(
              1, 2.0F, 'c', i, &j);
      // shouldnt be valid in this intermediate state
      CHECK(cc.is_not_valid());
      // but has a valid name
      CHECK(cc.concrete_connection_type() == "ConnectionWithFundamentalTypes");
      x.at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(not cc.is_not_valid());
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }

    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      // intended usage
      cc.with<::elastica::connections::ConnectionWithFundamentalTypes<>>(
            1, 2.0F, 'c', i, &j)
          .at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_connection_type() == "ConnectionWithFundamentalTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }
  }

  void test_connection_with_aggregate_types_via_forwarded_arguments() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      cc.with<::elastica::connections::ConnectionWithAggregateTypes<>>(
            "ODIs", "are", "good")
          .at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_connection_type() == "ConnectionWithAggregateTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }

    {  // xvalue
      ConcreteConnection cc(6UL, f, g, "test");
      auto x = cc.with<::elastica::connections::ConnectionWithAggregateTypes<>>(
          "Tests", "are", "better");
      CHECK(cc.is_not_valid());
      CHECK(cc.concrete_connection_type() == "ConnectionWithAggregateTypes");
      x.at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }
  }

  void test_connection_with_noncopyable_types_via_forwarded_arguments() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      // float r(2.0F), p(5.000F), q(8.000F);
      cc.with<::elastica::connections::ConnectionWithNonCopyableTypes<>>(
            std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
            std::make_unique<float>(80.0F))
          .at(::elastica::interface::pair(end - 5, 1UL));
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_connection_type() == "ConnectionWithNonCopyableTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }
  }

  void test_deferred_connection_with_aggregate_types_via_forwarded_arguments() {
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      cc.with<::elastica::connections::DeferredConnectionWithAggregateTypes<>>(
            "ODIs", "are", "good")
          .at(::elastica::interface::pair(defer, defer));
      CHECK(not cc.is_not_valid());
      CHECK(cc.concrete_connection_type() ==
            "DeferredConnectionWithAggregateTypes");
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
    }

    {  // xvalue
      ConcreteConnection cc(6UL, f, g, "test");
      auto x = cc.with<
          ::elastica::connections::DeferredConnectionWithAggregateTypes<>>(
          "Tests", "are", "better");
      CHECK(cc.is_not_valid());
      CHECK(cc.concrete_connection_type() ==
            "DeferredConnectionWithAggregateTypes");
      x.at(::elastica::interface::pair(defer, defer));
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      CHECK(not cc.is_not_valid());
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Connection.with_forwarded_arguments",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    test_connection_with_fundamental_types_via_forwarded_arguments();
    test_connection_with_aggregate_types_via_forwarded_arguments();
    test_connection_with_noncopyable_types_via_forwarded_arguments();
    test_deferred_connection_with_aggregate_types_via_forwarded_arguments();
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Connection.exception_for_incorrect_indices",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      ConcreteConnection cc(6UL, f, g, "test");

      CHECK_THROWS_NESTED_WITH(
          cc.unsafe_with(
              ::elastica::connections::ConnectionWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::pair(200UL, 1UL)),
          "Attempts to connect systems",
          "Number of degrees of freedom exceeded");

      CHECK_THROWS_NESTED_WITH(
          cc.unsafe_with(
              ::elastica::connections::ConnectionWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::pair(80UL, 80UL)),
          "Attempts to connect systems",
          "Number of degrees of freedom exceeded");

      CHECK_THROWS_NESTED_WITH(
          cc.unsafe_with(
              ::elastica::connections::ConnectionWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::pair(end - 400, 40UL)),
          "Attempts to connect systems",
          "Number of degrees of freedom exceeded");

      CHECK_THROWS_NESTED_WITH(
          cc.unsafe_with<>(
              ::elastica::connections::ConnectionWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::pair(100UL, end - 60)),
          "Attempts to connect systems",
          "Number of degrees of freedom exceeded");

      ConcreteConnection new_cc(6UL, f, g, "test");

      CHECK_THROWS_NESTED_WITH(
          cc.unsafe_with(
              ::elastica::connections::ConnectionWithNonCopyableTypes<>(
                  std::make_unique<float>(2.0F), std::make_unique<float>(5.0F),
                  std::make_unique<float>(80.0F)),
              ::elastica::interface::pair(end - 400, 1UL)),
          "Attempts to connect systems",
          "Number of degrees of freedom exceeded");

      //          Catch::Contains("Attempts to connect"));
    }
  }

  template <template <typename> class ConnectionType, typename... Args>
  std::unique_ptr<::elastica::connections::detail::ConnectionBase<int>>
  make_connection(Args&&... args) {
    return std::make_unique<ConnectionType<int>>(std::forward<Args>(args)...);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Connections.Connection.with_unique_ptr",
      "[Unit][Simulator][Interface][Connections]") {
    // clang-format on
    using ConcreteConnection =
        TestConnection<std::reference_wrapper<MockSystem>,
                       std::reference_wrapper<MockSystem>>;
    MockSystem f{100};
    MockSystem g{50};
    {  // rvalue
      int i(2);
      int j(4);
      ConcreteConnection cc(6UL, f, g, "test");
      CHECK(cc.is_not_valid());
      // registar type doesnt really matter here
      //      std::unique_ptr<::elastica::connections::detail::ConnectionBase<int>>
      //          runtime_connection = std::make_unique<
      //              ::elastica::connections::ConnectionWithFundamentalTypes<int>>(
      //              1, 2.0F, 'c', i, &j);
      auto runtime_connection = make_connection<
          ::elastica::connections::ConnectionWithFundamentalTypes>(1, 2.0F, 'c',
                                                                   i, &j);
      CHECK(f.n_indices == 100);
      CHECK(g.n_indices == 50);
      cc.with(std::move(runtime_connection),
              ::elastica::interface::pair(1UL, end - 4));
      CHECK(not cc.is_not_valid());
    }
  }

}  // namespace
