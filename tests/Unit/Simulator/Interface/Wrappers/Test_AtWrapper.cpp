//
// Created by Tejaswin Parthasarathy on 12/31/20.
//
#include <algorithm>
#include <iostream>
#include <type_traits>
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/AtWrapper.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/NamedType/TraitsTesters.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace ::elastica::interface;

  namespace {

    /// [AtWrapper_example]
    template <typename Callable>
    using AtWrappedType =
        named_type::NamedType<Callable, struct ParamTag, named_type::AtWrapper>;
    struct AnyCallable {
      constexpr int operator()(int x) const noexcept { return x; }
    };
    auto at_wrapped_callable = AtWrappedType<AnyCallable>(AnyCallable{});
    static_assert(at_wrapped_callable.at(2) == 2, "at wrapped failure!");
    /// [AtWrapper_example]

  }  // namespace

  decltype(auto) test_at_with_lvalue_func(int i) {
    /// [make_wrapped_at_example]
    auto some_callable = [i](int j) noexcept { return j + i; };
    auto callable_as_at = make_wrapped_at(some_callable);
    // use as callable_as_at.at(2);
    /// [make_wrapped_at_example]
    return callable_as_at;
  }

  decltype(auto) test_at_with_mutable_lvalue_func(int i) {
    auto x = [i](int j) mutable noexcept { return j + i; };
    return make_wrapped_at(x);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Wrappers.At.forwarded_arguments",
                     "[Unit][Simulator][Interface]") {
    {  // lvalue func
      auto wrapper(test_at_with_lvalue_func(1));
      {  // prvalues
        CHECK(wrapper.at(4) == 5);
        CHECK(wrapper.at(0) == 1);
        CHECK(wrapper.at(-10) == -9);
      }

      {  // lvalues
        int y = 2;
        CHECK(wrapper.at(y) == 3);
        CHECK(y == 2);
      }
    }

    {  // rvalue forward
      auto string_wrapper(make_wrapped_at([](std::string&& s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return std::move(s);
      }));
      CHECK(string_wrapper.at("best") == "BEST");
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Wrappers.At.mutable_forwarded_arguments",
      "[Unit][Simulator][Interface]") {
    {  // lvalue func
      auto wrapper(test_at_with_mutable_lvalue_func(1));
      //      auto wrapper = make_wrapped_at([i = 1](int j) mutable noexcept {
      //      return j + i; });

      {  // prvalues
        CHECK(wrapper.at(4) == 5);
        CHECK(wrapper.at(0) == 1);
        CHECK(wrapper.at(-10) == -9);
      }

      {  // lvalues
        int y = 2;
        CHECK(wrapper.at(y) == 3);
        CHECK(y == 2);
      }
    }
  }

  struct A {
    A(int o, float p, char q) : x(o), f(p), c(q){};
    ~A() = default;
    int x;
    float f;
    char c;
  };

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Wrappers.At.constructed_arguments",
      "[Unit][Simulator][Interface]") {
    auto a_wrapper(make_wrapped_at([](A a) {
      return std::string(MakeString{} << a.x << " " << a.f << " " << a.c);
    }));
    char c = 'c';
    float f(2.14F);
    auto test = std::string("1 2.14 c");
    CHECK(a_wrapper.at<A>(1, std::move(f), c) == test);
  }

  using testing::_1;
  using testing::CheckGuarantees;
  using AtAffordances = tmpl::list<named_type::AtWrapper<_1>>;
  auto lambda IGNORE_UNUSED = [](int x) { return x; };
  auto mut_lambda IGNORE_UNUSED = [i = 0]() mutable { return i; };
  struct MyCallable {
    int operator()(int x) { return 2 * x; }
  };
  struct MyConstCallable {
    int operator()(int x, int y) const { return 2 * x * y; }
  };

  using Callables =
      tmpl::list<decltype(lambda), decltype(mut_lambda), MyCallable,
                 MyConstCallable, std::function<void(int)>>;
  using CallablesCheck = CheckGuarantees<AtAffordances, Callables>;
  // check if all is true
  static_assert(tmpl::all<CallablesCheck>::value, "Oops!");

}  // namespace
