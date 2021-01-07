//
// Created by Tejaswin Parthasarathy on 1/1/21.
//
#include <tuple>
#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Utilities/NamedType/Affordances.hpp"
#include "Utilities/NamedType/NamedType.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace {

  using namespace named_type;

  namespace detail {
    // gettable checks
    template <class T>
    using Gettable_t = decltype(std::declval<T&>().get());

    // check non gettable
    using NGD = NamedType<double, struct DoubleTag>;
    static_assert(not ::tt::is_detected_v<Gettable_t, NGD>,
                  "should not be gettable!");
    using GD = NamedType<double, struct DoubleTag, Gettable>;
    static_assert(::tt::is_detected_v<Gettable_t, GD>, "should be gettable!");
    static_assert(std::is_same<std::decay_t<Gettable_t<GD>>, double>::value,
                  "should be gettable!");
  }  // namespace detail

  namespace detail {
    // check noncopyable namedtypes
    using CD = NamedType<double, struct DoubleTag>;
    static_assert(std::is_copy_constructible<CD>::value, "should be copyable!");
    static_assert(std::is_copy_assignable<CD>::value,
                  "should be copy assignable!");
    static_assert(std::is_move_constructible<CD>::value, "should be copyable!");
    static_assert(std::is_move_assignable<CD>::value,
                  "should be move assignable!");

    using NCD = NamedType<double, struct DoubleTag, NonCopyable>;
    /* Not really useful since it just checks for definition
     * https://devblogs.microsoft.com/oldnewthing/20190926-00/?p=102924
     * */
    //    static_assert(std::is_copy_constructible<NCD>::value,
    //                  "should be copyable!");
    //    static_assert(std::is_copy_assignable<NCD>::value,
    //                  "should be copy assignable!");
    //    static_assert(std::is_move_constructible<NCD>::value,
    //                  "should be copyable!");
    //    static_assert(std::is_move_assignable<NCD>::value,
    //                  "should be move assignable!");

    // TODO : Copying doesn't work for NCD, but how do we unit test this
    // behavior?
  }  // namespace detail

  namespace detail {

    // callable checks
    struct FirstCallable {
      int operator()(int x) const { return 2 * x; }
    };
    struct SecondCallable {
      double operator()(int x) const { return 3.0 * x; }
    };
    struct ComposedCallable {
      FirstCallable f{};
      SecondCallable s{};
      auto operator()(int x) const { return s(f(x)); }
    };
    // UDC
    struct D {
      explicit D(double dd) : d(dd){};
      explicit operator double() const { return d; }
      double d;
    };

    struct TemplatedCallable {
      template <typename DoubleLike>
      auto operator()(DoubleLike&& d) const {
        return 5.0 * static_cast<double>(d);
      }
    };
    struct MultiArgsCallable {
      FirstCallable f{};
      SecondCallable s{};
      ComposedCallable c{};

      auto operator()(int x, int y, int z) const {
        return std::make_tuple(f(x), s(y), c(z));
      }
    };

    template <typename T>
    struct ParameterFactory;

    template <typename T>
    using NamedCallable = NamedType<T, ParameterFactory<T>, Callable>;

    ELASTICA_TEST_CASE("Unit.Utilities.NamedType.Affordances.callable",
                       "[Unit][Utilities]") {
      NamedCallable<FirstCallable> f{FirstCallable()};
      CHECK(f(2) == 4);
      CHECK(f(-2) == -4);

      NamedCallable<SecondCallable> s{SecondCallable()};
      CHECK(s(2) == approx(6.0));
      CHECK(s(5.123) == approx(15.0));

      NamedCallable<ComposedCallable> c{ComposedCallable()};
      CHECK(c(2) == approx(12.0));
      CHECK(c(-2) == approx(-12.0));

      NamedCallable<MultiArgsCallable> m{MultiArgsCallable()};
      int x;
      double y, z;
      std::tie(x, y, z) = m(2, 3, 4);
      CHECK(x == 4);
      CHECK(y == approx(9.0));
      CHECK(z == approx(24.0));

      NamedCallable<TemplatedCallable> t{TemplatedCallable()};
      CHECK(t(D(5.0)) == approx(25.0));
    }

  }  // namespace detail

}  // namespace
