//
// Created by Tejaswin Parthasarathy on 12/30/20.
//

#include <algorithm>
#include <iostream>
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/WithWrapper.hpp"

namespace {

  using namespace ::elastica::interface;

  decltype(auto) test_with_with_lvalue_func(int i) {
    auto x = [i](int j) { return j + i; };
    return make_wrapped_with(x);
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

  // [[OutputRegex, 1 2.14 c]]
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Wrappers.With.constructed_arguments",
      "[Unit][Simulator][Interface]") {
    OUTPUT_TEST();
    auto a_wrapper(make_wrapped_with([](A a) {
      std::cout << a.x << " " << a.f << " " << a.c << std::endl;
    }));
    char c = 'c';
    float f(2.14F);
    a_wrapper.with<A>(1, std::move(f), c);
  }

}  // namespace
