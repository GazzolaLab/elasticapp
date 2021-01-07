//
// Created by Tejaswin Parthasarathy on 12/30/20.
//
#include <string>
#include <utility>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Options/Options.hpp"
#include "Options/Parameter.hpp"
#include "Utilities/NonCopyable.hpp"

namespace {

  /// [parameter_example]
  struct RealParameter : Options::Parameter<float> {
    using Parameter::Parameter;
    static constexpr Options::String help = {"real parameter"};
  };
  /// [parameter_example]

  /// [parameter_extra_example]
  struct AnotherRealParameter : Options::Parameter<float> {
    using Parameter::Parameter;
    static constexpr Options::String help = {"another real parameter"};
    // These are optional
    // you have access to typename "type"  from Parameter
    static type suggested_value() noexcept { return 3.0F; }
    static type lower_bound() noexcept { return 2.0F; }
    static type upper_bound() noexcept { return 10.0F; }
  };
  /// [parameter_extra_example]

  using builtin_types =
      tmpl::list<char, signed char, unsigned char, wchar_t, char16_t, char32_t,
                 int, unsigned int, long, unsigned long, long long,
                 unsigned long long, short, unsigned short, float, double,
                 long double, bool>;

  ELASTICA_TEST_CASE("Unit.Options.Parameter.types", "[Unit][Options]") {
    tmpl::for_each<builtin_types>([](auto t) {
      using builtin_type = tmpl::type_from<decltype(t)>;
      using parameter = Options::Parameter<builtin_type>;
      struct CustomParameter : parameter {};
      static_assert(
          std::is_same<builtin_type, typename CustomParameter::type>::value,
          "Oops!");
    });
    CHECK(true);
  }

  void test_parameter_construction_for_builtin_lvalues() {
    tmpl::for_each<builtin_types>([](auto t) {
      using builtin_type = tmpl::type_from<decltype(t)>;
      using parameter = Options::Parameter<builtin_type>;
      auto x(static_cast<builtin_type>(2.4151));
      parameter p(x);
      CHECK(p.t_ == x);
    });
  }

  void test_parameter_construction_for_builtin_prvalues() {
    tmpl::for_each<builtin_types>([](auto t) {
      using builtin_type = tmpl::type_from<decltype(t)>;
      using parameter = Options::Parameter<builtin_type>;
      parameter p(static_cast<builtin_type>(42.4151));
      CHECK(p.t_ == static_cast<builtin_type>(42.4151));
    });
  }

  void test_parameter_construction_for_builtin_xvalues() {
    tmpl::for_each<builtin_types>([](auto t) {
      using builtin_type = tmpl::type_from<decltype(t)>;
      using parameter = Options::Parameter<builtin_type>;
      auto x(static_cast<builtin_type>(21.4151));
      parameter p(std::move(x));
      CHECK(p.t_ == static_cast<builtin_type>(21.4151));
    });
  }

  void test_parameter_construction_for_strings() {
    using type = std::string;
    using parameter = Options::Parameter<type>;

    type a("looking good!");
    {  // lvalue
      parameter p(a);
      CHECK(p.t_ == a);
    }

    {  // prvalue
      parameter p("looking good!");
      CHECK(p.t_ == a);
    }

    {  // xvalue
      parameter p(std::move(a));
      CHECK(p.t_ == "looking good!");
    }
  }

  void test_parameter_construction_for_vectors() {
    using type = std::vector<std::string>;
    using parameter = Options::Parameter<type>;

    type a{"looking", "good", "boi!"};
    {  // lvalue
      parameter p(a);
      CHECK(p.t_ == a);
    }

    {  // prvalue
      parameter p({"looking", "good", "boi!"});
      CHECK(p.t_ == a);
    }

    {  // xvalue
      parameter p(std::move(a));
      CHECK(p.t_ == type{"looking", "good", "boi!"});
    }
  }

  ELASTICA_TEST_CASE("Unit.Options.Parameter.constructors", "[Unit][Options]") {
    // builtins
    test_parameter_construction_for_builtin_lvalues();
    test_parameter_construction_for_builtin_prvalues();
    test_parameter_construction_for_builtin_xvalues();

    // aggregates
    test_parameter_construction_for_strings();
    test_parameter_construction_for_vectors();
    // todo : add more types
  }

  void test_parameter_explicit_conversion() {
    RealParameter r(2.0F);
    RealParameter::type f(r);
    REQUIRE(f == Approx(2.0F));

    RealParameter::type g(RealParameter(2.0F));
    REQUIRE(g == Approx(2.0F));
  }

  struct NCT : elastica::NonCopyable {
    NCT() = default;
    NCT(std::string s) : s_(std::move(s)){};
    ~NCT() = default;
    NCT(NCT&& other) noexcept : s_(std::move(other.s_)){};
    std::string s_{};
  };

  struct NCT2 : elastica::NonCopyable {
    NCT2(NCT n) : n_(std::move(n)){};
    ~NCT2() = default;
    NCT2(NCT2&& other) noexcept : n_(std::move(other.n_)){};
    NCT n_{};
  };

  void test_parameter_explicit_conversion_for_noncopyable_types() {
    {
      using type = NCT;
      using parameter = Options::Parameter<type>;
      /* intent
      parameter p(std::string("Its a good day!"));
      // parameter p("Its a good day!"); // doesnt work because C++ allows only
      one implicit conversion!
       parameter::type f(p);
      */
      std::string s("Its a good day!");
      // NCT, but constructed from a string->NCT->Parameter and converted to NCT
      type f(parameter{s});
      CHECK(f.s_ == s);
    }
    {
      using type = NCT2;
      using parameter = Options::Parameter<type>;
      auto s = std::string("Its a good day!");
      type f(parameter{NCT(s)});
      CHECK(f.n_.s_ == s);
    }
  }

  ELASTICA_TEST_CASE("Unit.Options.Parameter.explicit_conversion",
                     "[Unit][Options]") {
    // This test is useful for the internal workings of the Options library
    test_parameter_explicit_conversion();
    test_parameter_explicit_conversion_for_noncopyable_types();
  }

  float check_conversion(RealParameter const& p) { return p.t_ + 1.0F; }

  float check_conversion(Options::Parameter<float> const& q) {
    return q.t_ + 2.0F;
  }

  ELASTICA_TEST_CASE("Unit.Options.Parameter.cross_conversion",
                     "[Unit][Options]") {
    RealParameter p(2.0F);
    AnotherRealParameter q(2.0F);
    REQUIRE(3.0F == approx(check_conversion(p)));
    REQUIRE(4.0F == approx(check_conversion(q)));
  }

}  // namespace