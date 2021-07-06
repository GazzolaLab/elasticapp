//
// Created by Tejaswin Parthasarathy on 12/30/20.
//

#include <algorithm>
#include <iostream>
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/WithWrapper.hpp"
#include "Utilities/MakeString.hpp"

namespace {

  namespace {

    /// [WithWrapper_example]
    template <typename Callable>
    using WithWrappedType = named_type::NamedType<Callable, struct ParamTag,
                                                  named_type::WithWrapper>;
    struct AnyCallable {
      constexpr int operator()(int x) const noexcept { return x; }
    };
    auto with_wrapped_callable = WithWrappedType<AnyCallable>(AnyCallable{});
    static_assert(with_wrapped_callable.with(2) == 2, "with wrapped failure!");
    /// [WithWrapper_example]

  }  // namespace

  using namespace ::elastica::interface;

  decltype(auto) test_with_with_lvalue_func(int i) {
    /// [make_wrapped_with_example]
    auto some_callable = [i](int j) noexcept { return j + i; };
    auto callable_as_with = make_wrapped_with(some_callable);
    // use as callable_as_with.with(2);
    /// [make_wrapped_with_example]
    return callable_as_with;
  }

  decltype(auto) test_with_with_mutable_lvalue_func(int i) {
    auto x = [i](int j) mutable noexcept { return j + i; };
    return make_wrapped_with(x);
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Wrappers.With.forwarded_arguments",
      "[Unit][Simulator][Interface]") {
    {  // lvalue func
      auto wrapper(test_with_with_lvalue_func(1));
      {  // prvalues
        CHECK(wrapper.with(4) == 5);
        CHECK(wrapper.with(0) == 1);
        CHECK(wrapper.with(-10) == -9);
      }

      {  // lvalues
        int y = 2;
        CHECK(wrapper.with(y) == 3);
        CHECK(y == 2);
      }
    }

    {  // rvalue forward
      auto string_wrapper(make_wrapped_with([](std::string&& s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return std::move(s);
      }));
      CHECK(string_wrapper.with("best") == "BEST");
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Wrappers.With.mutable_forwarded_arguments",
      "[Unit][Simulator][Interface]") {
    {  // lvalue func
      auto wrapper(test_with_with_mutable_lvalue_func(1));
      {  // prvalues
        CHECK(wrapper.with(4) == 5);
        CHECK(wrapper.with(0) == 1);
        CHECK(wrapper.with(-10) == -9);
      }

      {  // lvalues
        int y = 2;
        CHECK(wrapper.with(y) == 3);
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
      "Unit.Simulator.Interface.Wrappers.With.constructed_arguments",
      "[Unit][Simulator][Interface]") {
    auto a_wrapper(make_wrapped_with([](A a) {
      return std::string(MakeString{} << a.x << " " << a.f << " " << a.c);
    }));
    char c = 'c';
    float f(2.14F);
    auto test = std::string("1 2.14 c");
    CHECK(a_wrapper.with<A>(1, std::move(f), c) == test);
  }

}  // namespace
