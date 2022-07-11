//
// Created by Tejaswin Parthasarathy on 6/22/21.
//
#include <algorithm>
#include <iostream>
#include <type_traits>
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/ToWrapper.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/NamedType/TraitsTesters.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace ::elastica::interface;

  namespace {

    /// [ToWrapper_example]
    template <typename Callable>
    using ToWrappedType =
        named_type::NamedType<Callable, struct ParamTag, named_type::ToWrapper>;
    struct AnyCallable {
      constexpr int operator()(int x) const noexcept { return x; }
    };
    auto to_wrapped_callable = ToWrappedType<AnyCallable>(AnyCallable{});
    static_assert(to_wrapped_callable.to(2) == 2, "to wrapped failure!");
    /// [ToWrapper_example]

  }  // namespace

  decltype(auto) test_to_with_lvalue_func(int i) {
    /// [make_wrapped_to_example]
    auto some_callable = [i](int j) noexcept { return j + i; };
    auto callable_as_to = make_wrapped_to(some_callable);
    // use as callable_as_to.to(2);
    /// [make_wrapped_to_example]
    return callable_as_to;
  }

  decltype(auto) test_to_with_mutable_lvalue_func(int i) {
    auto x = [i](int j) mutable noexcept { return j + i; };
    return make_wrapped_to(x);
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Wrappers.To.forwarded_arguments",
                     "[Unit][Simulator][Interface]") {
    {  // lvalue func
      auto wrapper(test_to_with_lvalue_func(1));
      {  // prvalues
        CHECK(wrapper.to(4) == 5);
        CHECK(wrapper.to(0) == 1);
        CHECK(wrapper.to(-10) == -9);
      }

      {  // lvalues
        int y = 2;
        CHECK(wrapper.to(y) == 3);
        CHECK(y == 2);
      }
    }

    {  // rvalue forward
      auto string_wrapper(make_wrapped_to([](std::string&& s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return std::move(s);
      }));
      CHECK(string_wrapper.to("best") == "BEST");
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Wrappers.To.mutable_forwarded_arguments",
      "[Unit][Simulator][Interface]") {
    {  // lvalue func
      auto wrapper(test_to_with_mutable_lvalue_func(1));
      //      auto wrapper = make_wrapped_to([i = 1](int j) mutable noexcept {
      //      return j + i; });

      {  // prvalues
        CHECK(wrapper.to(4) == 5);
        CHECK(wrapper.to(0) == 1);
        CHECK(wrapper.to(-10) == -9);
      }

      {  // lvalues
        int y = 2;
        CHECK(wrapper.to(y) == 3);
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
      "Unit.Simulator.Interface.Wrappers.To.constructed_arguments",
      "[Unit][Simulator][Interface]") {
    auto a_wrapper(make_wrapped_to([](A a) {
      return std::string(MakeString{} << a.x << " " << a.f << " " << a.c);
    }));
    char c = 'c';
    float f(2.14F);
    auto test = std::string("1 2.14 c");
    CHECK(a_wrapper.to<A>(1, std::move(f), c) == test);
  }

  using testing::_1;
  using testing::CheckGuarantees;
  using ToAffordances = tmpl::list<named_type::ToWrapper<_1>>;
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
  using CallablesCheck = CheckGuarantees<ToAffordances, Callables>;
  // check if all is true
  static_assert(tmpl::all<CallablesCheck>::value, "Oops!");

}  // namespace
