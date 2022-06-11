//
// Created by Tejaswin Parthasarathy on 12/31/20.
//
#include <string>
#include <utility>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Utilities/NamedType/Affordances.hpp"
#include "Utilities/NamedType/NamedType.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace named_type;

  namespace {
    // Check for EBCO
    using Meter = NamedType<double, struct MeterParameter>;
    static_assert(sizeof(Meter) == sizeof(double), "Oops!");

    using BetterMeter =
        NamedType<long long, struct MeterParameter, Gettable, NonCopyable>;
    static_assert(sizeof(Meter) == sizeof(long long), "Oops!");

  }  // namespace

  void test_named_type_constructors_for_basic_types() {
    using basic_types = tmpl::list<int, char, float, double>;

    tmpl::for_each<basic_types>([](auto t) {
      using basic_type = tmpl::type_from<decltype(t)>;
      using Width = NamedType<basic_type, struct WidthParameter>;
      using GettableWidth =
          NamedType<basic_type, struct AnotherParameter, Gettable>;
      {  // lvalue constructor
        auto val(static_cast<basic_type>(3.14159));
        Width w(val);
        GettableWidth gw(val);
        CHECK(gw.get() == approx(val));
      }
    });
  }

  void test_named_type_constructors_for_strings() {
    using aggregate_type = std::string;
    using P = NamedType<aggregate_type, struct Parameter>;
    using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

    auto val(static_cast<aggregate_type>("Aggregate test"));
    {  // lvalue constructor
      P p(val);
      GP gp(val);
      CHECK(gp.get() == val);
    }

    {  // prvalue
      P p("What's this");
      GP gp("Aggregate test");
      CHECK(gp.get() == val);
    }

    {  // xvalue
      auto val_copy(val);
      GP gp(std::move(val_copy));
      CHECK(gp.get() == val);
    }
  }

  void test_named_type_constructors_for_vectors() {
    using aggregate_type = std::vector<int>;
    using P = NamedType<aggregate_type, struct Parameter>;
    using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

    aggregate_type val{1, 2, 3, 4, 5, 6};
    {  // lvalue constructor
      P p(val);
      GP gp(val);
      CHECK(gp.get() == val);
    }

    {  // prvalue
      P p{{1, 2, 3, 4, 5, 6}};
      GP gp{{1, 2, 3, 4, 5, 6}};
      CHECK(gp.get() == val);
    }

    {  // xvalue
      auto val_copy(val);
      GP gp(std::move(val_copy));
      CHECK(gp.get() == val);
    }
  }

  struct Aggregate {
    explicit Aggregate(std::string arg) : d(22.1), s(std::move(arg)){};
    ~Aggregate() = default;
    // whoa! sneaky sneaky
    Aggregate(Aggregate const& other) : d(other.d + 1.0), s(other.s){};
    Aggregate(Aggregate&& other) noexcept : d(other.d), s(std::move(other.s)){};

    friend bool operator==(Aggregate const& first, Aggregate const& second) {
      return first.d == approx(second.d) && first.s == second.s;
    }

    double d;
    std::string s;
  };

  void check_aggregate(const std::vector<Aggregate>& actual,
                       const std::vector<Aggregate>& expected) {
    using SV = std::vector<std::string>;
    using DV = std::vector<double>;
    auto get_s = [](Aggregate const& agg) { return agg.s; };
    auto get_d = [](Aggregate const& agg) { return agg.d; };

    SV actual_string, expected_string;
    DV actual_double, expected_double;
    std::transform(actual.cbegin(), actual.cend(),
                   std::back_inserter(actual_string), get_s);
    std::transform(actual.cbegin(), actual.cend(),
                   std::back_inserter(actual_double), get_d);
    std::transform(expected.cbegin(), expected.cend(),
                   std::back_inserter(expected_string), get_s);
    std::transform(expected.cbegin(), expected.cend(),
                   std::back_inserter(expected_double), get_d);

    CHECK_ITERABLE_APPROX(actual_double, expected_double);
    CHECK(actual_string == expected_string);
  }

  void test_named_type_constructors_for_custom() {
    using aggregate_type = std::vector<Aggregate>;
    using P = NamedType<aggregate_type, struct Parameter>;
    using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

    aggregate_type val{Aggregate("This"), Aggregate("is"),
                       Aggregate("painful")};

    auto val_copy(val);

    {  // lvalue constructor
      P p(val);
      GP gp(val);
      CHECK(gp.get() != val);
      check_aggregate(gp.get(), val_copy);
      //      CHECK(gp.get() == val_copy);
    }

    {  // xvalue
      GP gp(std::move(val_copy));
      auto val_second_copy(val);
      CHECK(gp.get() != val);
      //      CHECK(gp.get() == val_second_copy);
      check_aggregate(gp.get(), val_second_copy);
    }
  }

  void test_named_type_constructors_for_aggregate_types() {
    test_named_type_constructors_for_strings();
    test_named_type_constructors_for_vectors();
    test_named_type_constructors_for_custom();
  }

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.constructor",
                     "[Unit][Utilities]") {
    test_named_type_constructors_for_basic_types();
    test_named_type_constructors_for_aggregate_types();
  }

  void test_named_type_copy_constructors_for_basic_types() {
    using basic_types = tmpl::list<int, char, float, double>;

    tmpl::for_each<basic_types>([](auto t) {
      using basic_type = tmpl::type_from<decltype(t)>;
      using GP = NamedType<basic_type, struct AnotherParameter, Gettable>;
      auto val(static_cast<basic_type>(3.14159));
      GP gw(val);
      auto gw2(gw);
      CHECK(gw2.get() == approx(val));
    });
  }

  void test_named_type_copy_constructors_for_aggregate_types() {
    {
      using aggregate_type = std::string;
      using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

      auto val(static_cast<aggregate_type>("Aggregate test"));
      GP gp(val);
      auto gp2(gp);
      CHECK(gp2.get() == (val));
    }

    {
      using aggregate_type = std::vector<double>;
      using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

      aggregate_type val{1.24, 22.0 / 7.0, 32.9};
      GP gp(val);
      auto gp2(gp);
      CHECK_ITERABLE_APPROX(gp2.get(), val);
      //      CHECK(gp2.get() == (val));
    }

    {
      using aggregate_type = std::vector<Aggregate>;
      using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

      aggregate_type val{Aggregate("This"), Aggregate("is"),
                         Aggregate("painful")};
      // 1 copy
      GP gp(val);
      // Another copy
      auto gp2 = gp;
      auto const val2 = val;   // force a copy here
      auto const val3 = val2;  // force a copy here
      CHECK(gp2.get() != (val));
      //      CHECK(gp2.get() == (val2));
      check_aggregate(gp2.get(), val3);
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.copy_constructor",
                     "[Unit][Utilities]") {
    test_named_type_copy_constructors_for_basic_types();
    test_named_type_copy_constructors_for_aggregate_types();
  }

  void test_named_type_move_constructors_for_basic_types() {
    using basic_types = tmpl::list<int, char, float, double>;

    tmpl::for_each<basic_types>([](auto t) {
      using basic_type = tmpl::type_from<decltype(t)>;
      using GP = NamedType<basic_type, struct AnotherParameter, Gettable>;
      auto val(static_cast<basic_type>(3.14159));
      GP gw(val);
      auto gw2(std::move(gw));
      CHECK(gw2.get() == approx(val));
    });
  }

  void test_named_type_move_constructors_for_aggregate_types() {
    {
      using aggregate_type = std::string;
      using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

      auto val(static_cast<aggregate_type>("Aggregate test"));
      GP gp(val);
      auto gp2(std::move(gp));
      CHECK(gp2.get() == (val));
    }

    {
      using aggregate_type = std::vector<double>;
      using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

      aggregate_type val{1.24, 22.0 / 7.0, 32.9};
      GP gp(val);
      auto gp2(std::move(gp));
      CHECK_ITERABLE_APPROX(gp2.get(), val);
      //      CHECK(gp2.get() == approx(val));
    }

    {
      using aggregate_type = std::vector<Aggregate>;
      using GP = NamedType<aggregate_type, struct AnotherParameter, Gettable>;

      aggregate_type val{Aggregate("This"), Aggregate("is"),
                         Aggregate("painful")};

      // 1 copy
      GP gp(val);
      // copies each value?
      auto gp2(std::move(gp));
      // 1 copy
      auto const val2(val);
      CHECK(gp2.get() != (val));
      check_aggregate(gp2.get(), (val2));
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.move_constructor",
                     "[Unit][Utilities]") {
    test_named_type_move_constructors_for_basic_types();
    test_named_type_move_constructors_for_aggregate_types();
  }

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.set", "[Unit][Utilities]") {
    using basic_types = tmpl::list<int, char, float, double>;

    tmpl::for_each<basic_types>([](auto t) {
      using basic_type = tmpl::type_from<decltype(t)>;
      // using P = NamedType<basic_type, struct Parameter>;
      using SettableP = NamedType<basic_type, struct Parameter, Gettable>;
      {  // lvalue constructor
        auto val(static_cast<basic_type>(3.14159));
        SettableP gw(val);
        auto new_val(static_cast<basic_type>(32.1));
        gw.get() = new_val;
        CHECK(gw.get() == approx(new_val));
      }
    });
  }

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.set_via_reference",
                     "[Unit][Utilities]") {
    using basic_type = std::string;
    using reference = std::reference_wrapper<basic_type>;
    using NamedRef = NamedType<reference, struct Parameter, Gettable>;
    std::string value = "value1";
    auto change_value = [](NamedRef m) { m.get().get() = "value2"; };
    change_value(NamedRef(value));
    REQUIRE(value == "value2");
  }

  // https://github.com/joboccara/NamedType/blob/master/test/tests.cpp
  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.constexpr",
                     "[Unit][Utilities]") {
    EMPTY_TEST();

    using strong_bool = NamedType<bool, struct BoolTag, Gettable>;

    static_assert(strong_bool{true}.get(), "NamedType is not constexpr");
  }

  // https://github.com/joboccara/NamedType/blob/master/test/tests.cpp
  struct throw_on_construction {
    [[noreturn]] throw_on_construction() { throw 42; }
    [[noreturn]] explicit throw_on_construction(int) { throw "exception"; }
  };

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.noexcept", "[Unit][Utilities]") {
    using C = NamedType<throw_on_construction, struct throwTag>;
    using strong_int = NamedType<int, struct IntTag>;

    CHECK(noexcept(strong_int{32}));
    CHECK(!noexcept(C(throw_on_construction())));

    CHECK(noexcept(strong_int(3)));
    CHECK(!noexcept(C(throw_on_construction(32))));
  }

}  // namespace
