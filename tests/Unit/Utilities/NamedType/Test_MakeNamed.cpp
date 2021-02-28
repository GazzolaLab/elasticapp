//
// Created by Tejaswin Parthasarathy on 12/31/20.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/AtWrapper.hpp"
#include "Utilities/NamedType/Affordances.hpp"
#include "Utilities/NamedType/MakeNamed.hpp"
#include "Utilities/NamedType/NamedType.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace {

  using namespace named_type;

  /// [arithmetic_example]
  template <typename Arith>
  using Arithmetic = NamedType<Arith, struct ArithmeticParameter, Gettable>;
  /// [arithmetic_example]

  // UDC
  struct D {
    explicit D(double dd) : d(dd){};
    explicit operator double() const { return d; }
    double d;
  };

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.named_generics",
                     "[Unit][Utilities]") {
    using basic_types = tmpl::list<int, float, double, D>;
    tmpl::for_each<basic_types>([](auto t) {
      using basic_type = tmpl::type_from<decltype(t)>;
      {  // lvalue constructor
        auto val(static_cast<basic_type>(3.14159));

        /// [make_arithmetic_example]
        auto n = make_named<Arithmetic>(val);
        /// [make_arithmetic_example]

        CHECK(approx(n.get()) == val);
        static_assert(
            cpp17::is_same_v<basic_type, typename decltype(n)::UnderlyingType>,
            "Oops!");
      }
    });
  }

  template <typename T>
  struct ParameterFactory;

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.named_factory",
                     "[Unit][Utilities]") {
    using basic_types = tmpl::list<int, float, double, D>;
    tmpl::for_each<basic_types>([](auto t) {
      using basic_type = tmpl::type_from<decltype(t)>;
      using parameter_type = ParameterFactory<basic_type>;
      {  // lvalue constructor
        auto val(static_cast<basic_type>(3.14159));
        auto n = make_named<parameter_type, Gettable>(val);
        CHECK(approx(n.get()) == val);
        static_assert(
            cpp17::is_same_v<basic_type, typename decltype(n)::UnderlyingType>,
            "Oops!");
        static_assert(
            cpp17::is_same_v<parameter_type, typename decltype(n)::TagType>,
            "Oops!");
      }
    });
  }

  struct NCD : public ::elastica::NonCopyable {
    NCD(double a) : d(a){};
    double d;
  };

  void test_rvalues() {
    auto n = make_named_with_default<Gettable>(NCD(2.0));
    CHECK(approx(n.get().d) == 2.0);
    auto b(NCD(2.341234));
    auto nc = make_named_with_default<Gettable, NonCopyable>(std::move(b));
    CHECK(approx(nc.get().d) == 2.341234);
  }

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.named_factory_with_default",
                     "[Unit][Utilities]") {
    using basic_types = tmpl::list<int, float, double, D>;
    tmpl::for_each<basic_types>([](auto t) {
      using basic_type = tmpl::type_from<decltype(t)>;
      {  // lvalue constructor
        auto val(static_cast<basic_type>(3.14159));
        auto n = make_named_with_default<Gettable>(val);
        CHECK(approx(n.get()) == val);
        static_assert(
            cpp17::is_same_v<basic_type, typename decltype(n)::UnderlyingType>,
            "Oops!");
        static_assert(cpp17::is_same_v<_DefaultNamedTypeParameterTag_,
                                       typename decltype(n)::TagType>,
                      "Oops!");
      }
    });

    test_rvalues();
  }

  struct throw_on_construction {
    [[noreturn]] throw_on_construction() { throw 42; }
    [[noreturn]] explicit throw_on_construction(int) { throw "exception"; }
  };

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.named_factory_noexcept",
                     "[Unit][Utilities]") {
    CHECK(
        !noexcept(make_named<ParameterFactory<int>>(throw_on_construction())));
    CHECK(!noexcept(
        make_named<ParameterFactory<int>, Gettable>(throw_on_construction())));
    CHECK(!noexcept(make_named_with_default(throw_on_construction())));
    CHECK(
        !noexcept(make_named_with_default<Gettable>(throw_on_construction())));
    CHECK(!noexcept(
        make_named<ParameterFactory<int>>(throw_on_construction(43))));
    CHECK(!noexcept(
        make_named_with_default<Gettable>(throw_on_construction(89))));

    CHECK(noexcept(make_named_with_default<Gettable>(2.0F)));
    CHECK(noexcept(make_named<ParameterFactory<float>>(2ULL)));
  }

  ELASTICA_TEST_CASE("Unit.Utilities.NamedType.named_callables",
                     "[Unit][Utilities]") {
    // callable checks
    struct FirstCallable {
      int operator()(int x) const { return 2 * x; }
    };
    auto n = make_named<struct FirstCallableTag, Callable>(FirstCallable());
    CHECK(approx(n(5)) == 10);

    struct SecondCallable {
      int t;
      auto make_it() {
        return make_named_with_default<Callable>(
            [this](int x) -> int { return t * x; });
        //        return make_wrapped_at([this](int x) -> int { return t * x;
        //        });
      }
    };
    SecondCallable s{3};
    CHECK(approx(s.make_it()(5)) == 15);
  }

}  // namespace