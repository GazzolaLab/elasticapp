//
// Created by Tejaswin Parthasarathy on 2/11/21.
//
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Utilities/MakeNamedFunctor.hpp"
#include "Utilities/NamedType.hpp"

namespace {

  using namespace named_type;

  template <typename Func>
  using FuncCallable = NamedType<Func, struct CallableParameter, Callable>;

  ELASTICA_TEST_CASE("Unit.Utilities.named_functor", "[Unit][Utilities]") {
    {  // one function
      auto int_callable =
          make_named_functor<FuncCallable>([](int i) -> int { return i + 1; });
      CHECK(int_callable(5) == 6);
      CHECK(int_callable(5.3) == 6);
      CHECK(int_callable(4.9) == 5);
    }

    {  // multiple
      auto arithmetic_callable = make_named_functor<FuncCallable>(
          [](int i) -> int { return i + 1; },
          [](double d) -> double { return d + 1.0; });
      CHECK(arithmetic_callable(5) == 6);
      CHECK(arithmetic_callable(5.3) == approx(6.3));
      CHECK(arithmetic_callable(4.9) == approx(5.9));
    }

    {
      auto constant_callables = make_named_functor<FuncCallable>(
          [](std::integral_constant<int, 0>) { return 0; },
          [](std::integral_constant<int, 1>) { return 1; },
          [](std::integral_constant<int, 2>) { return 2; },
          [](std::integral_constant<int, 3>) { return 3; },
          [](std::integral_constant<int, 4>) { return 4; },
          [](std::integral_constant<int, 5>) { return 5; },
          [](std::integral_constant<int, 6>) { return 6; },
          [](std::integral_constant<int, 7>) { return 7; },
          [](std::integral_constant<int, 8>) { return 8; },
          [](std::integral_constant<int, 9>) { return 9; },
          [](std::integral_constant<int, 10>) { return 10; });
      CHECK((0 == constant_callables(std::integral_constant<int, 0>{})));
      CHECK((1 == constant_callables(std::integral_constant<int, 1>{})));
      CHECK((2 == constant_callables(std::integral_constant<int, 2>{})));
      CHECK((3 == constant_callables(std::integral_constant<int, 3>{})));
      CHECK((4 == constant_callables(std::integral_constant<int, 4>{})));
      CHECK((5 == constant_callables(std::integral_constant<int, 5>{})));
      CHECK((6 == constant_callables(std::integral_constant<int, 6>{})));
      CHECK((7 == constant_callables(std::integral_constant<int, 7>{})));
      CHECK((8 == constant_callables(std::integral_constant<int, 8>{})));
      CHECK((9 == constant_callables(std::integral_constant<int, 9>{})));
      CHECK((10 == constant_callables(std::integral_constant<int, 10>{})));
    }
  }

}  // namespace