//
// Created by Tejaswin Parthasarathy on 12/31/20.
//
#include <algorithm>
#include <iostream>
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Wrappers/AtWrapper.hpp"
#include "Utilities/TMPL.hpp"
#include "tests/Unit/Utilities/NamedType/TraitsTesters.hpp"

namespace {

  using namespace ::elastica::interface;

  decltype(auto) test_at_with_lvalue_func(int i) {
    auto x = [i](int j) noexcept { return j + i; };
    return make_wrapped_at(x);
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

  // [[OutputRegex, 1 2.14 c]]
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Interface.Wrappers.At.constructed_arguments",
      "[Unit][Simulator][Interface]") {
    OUTPUT_TEST();
    auto a_wrapper(make_wrapped_at([](A a) {
      std::cout << a.x << " " << a.f << " " << a.c << std::endl;
    }));
    char c = 'c';
    float f(2.14F);
    a_wrapper.at<A>(1, std::move(f), c);
  }

  using testing::_1;
  using testing::CheckGuarantees;
  using AtAffordances = tmpl::list<named_type::AtWrapper<_1>>;
  auto lambda = [](int x) { return x; };
  auto mut_lambda = [i = 0]() mutable { return i; };
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
