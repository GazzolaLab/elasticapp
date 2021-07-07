//
// Created by Tejaswin Parthasarathy on 1/3/21.
//

#include <functional>

#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/IsCallable.hpp"

namespace {

  struct Callable {
    void operator()() {}
  };

  struct PrivateCallable {
   private:
    void operator()(int) {}
    double operator()(bool, char*) { return 2.0; }
    PrivateCallable(int, int) {}
  };

  struct NotCallable {
    void mFoo() {}
    void mConstFoo() const {}
    int mInt;
  };

  int foo() { return int{}; }

  template <typename... Args>
  void bar(Args&&..., ...) {}

  auto IGNORE_UNUSED lamb = [](char**) -> decltype(auto) {
    return "Hello World!";
  };

  auto IGNORE_UNUSED templated_lamb = [](auto, auto) -> decltype(auto) {
    return "Hello World!";
  };

  auto IGNORE_UNUSED variadic_templated_lamb = [](auto...) -> decltype(auto) {
    return "Hello World!";
  };

  enum WeakTypeEnum {};
  enum class StrongTypeEnum {};

  template <typename T>
  struct CheckNotCallable : std::true_type {
    static_assert(not tt::is_callable<T>::value,
                  "A non-callable was recognized as callable");
  };

  template <typename T>
  struct CheckCallable : std::true_type {
    static_assert(tt::is_callable<T>::value,
                  "A callable was recognized as a non-callable");
  };

  using expected_negative_types =
      tmpl::list<int, double, long long, long double, int*, int const*,
                 int const* const, WeakTypeEnum, StrongTypeEnum,
                 decltype(&NotCallable::mInt), decltype(nullptr), Callable*>;
  using expected_negative_references =
      tmpl::list<int&, int const&, NotCallable&, NotCallable const&>;
  using expected_negative_functions =
      tmpl::list<decltype(&NotCallable::mFoo),
                 decltype(&NotCallable::mConstFoo)>;

  int (&rfoo)() = foo;
  auto IGNORE_UNUSED pfoo = &foo;
  using expected_positive_references =
      tmpl::list<Callable&, Callable const&, decltype(rfoo)>;
  using expected_positive_functions =
      tmpl::list<int(), double(char*), void(bool) const, void(bool)&,
                 void(bool)&&, void(...), decltype(bar<>), decltype(pfoo),
                 Callable, PrivateCallable, std::function<int()>,
                 decltype(lamb), decltype(templated_lamb),
                 decltype(variadic_templated_lamb)>;

  using negatives =
      tmpl::append<expected_negative_references, expected_negative_types,
                   expected_negative_functions>;
  using positives =
      tmpl::append<expected_positive_references, expected_positive_functions>;

  using negative_checks IGNORE_UNUSED =
      tmpl::transform<negatives, CheckNotCallable<tmpl::_1>>;

  using positive_checks IGNORE_UNUSED =
      tmpl::transform<positives, CheckCallable<tmpl::_1>>;

  /// [is_callable_example]
  static_assert(tt::is_callable<std::function<int()>>::value,
                "Failed testing type trait is_callable<function>");
  auto IGNORE_UNUSED my_lambda = [](int x) { return x + 1; };
  static_assert(tt::is_callable<decltype(my_lambda)>::value,
                "Failed testing type trait is_callable<lambda>");
  auto IGNORE_UNUSED my_templated_lambda = [](auto x, auto y) { return x * y; };
  static_assert(tt::is_callable<decltype(my_templated_lambda)>::value,
                "Failed testing type trait is_callable<templated_lambda>");
  /// [is_callable_example]

}  // namespace