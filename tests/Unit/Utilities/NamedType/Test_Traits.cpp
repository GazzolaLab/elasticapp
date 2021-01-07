//
// Created by Tejaswin Parthasarathy on 12/31/20.
//
#include <functional>
#include <string>

#include "TraitsTesters.hpp"
#include "Utilities/NamedType/Affordances.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace named_type;

  using testing::_1;
  using testing::CheckGuarantees;

  // user defined struct
  struct A {};
  using basic_types = tmpl::list<int, bool, std::string, A>;

  namespace {
    using NoGuaranteeAffordances = tmpl::list<Gettable<_1>, NonCopyable<_1>>;
    using NoGuaranteeCheck =
        CheckGuarantees<NoGuaranteeAffordances, basic_types>;

    // check if all is true
    static_assert(tmpl::all<NoGuaranteeCheck>::value, "Oops!");
  }  // namespace

  namespace {

    using CallableAffordances = tmpl::list<Callable<_1>>;
    using NoCallableCheck = CheckGuarantees<CallableAffordances, basic_types>;
    // its bad if even one of them is a true type
    static_assert(
        tmpl::any<NoCallableCheck, tmpl::not_<tmpl::detail::non_null>>::value,
        "Oops!");

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
    using CallablesCheck = CheckGuarantees<CallableAffordances, Callables>;
    // check if all is true
    static_assert(tmpl::all<CallablesCheck>::value, "Oops!");

  }  // namespace

}  // namespace