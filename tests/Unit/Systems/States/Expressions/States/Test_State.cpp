//
// Created by Tejaswin Parthasarathy on 5/24/21.
//
#include <string>
#include <type_traits>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Systems/States/Expressions/States.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/Types.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  using ::elastica::Time;
  using ::elastica::TimeDelta;

  // for a fresh state whose + operator is not defined
  template <typename T,
            typename OrderTag = elastica::states::tags::PrimitiveTag>
  struct Holder {
    using ElementType = T;
    using Order = OrderTag;

    Holder() : t_(){};
    Holder(T t) : t_(std::move(t)){};

    //    template <typename... Args>
    //    Holder(Args&&... args) : t_(std::forward<Args>(args)...) {}

    template <typename Arg, typename SomeOrderTag>
    Holder& operator=(Holder<Arg, SomeOrderTag> const& arg) {
      t_ = arg.t_;
      return *this;
    }

    //    template <typename Arg>
    //    Holder& operator=(Arg&& arg) {
    //      t_ = std::forward<Arg>(arg);
    //      return *this;
    //    };

    // needs a size member
    auto size() const noexcept -> std::size_t { return 1UL; }

    template <typename Arg, typename SomeOrderTag>
    Holder& operator+=(Holder<Arg, SomeOrderTag> const& other) {
      t_ += other.t_;
      return *this;
    }

    T t_;
  };

  struct CountedString;

  // declare a + operation to put the results into
  template <typename T,
            Requires<not cpp17::is_same_v<T, CountedString>> = nullptr>
  Holder<T, elastica::states::tags::PrimitiveTag> operator+(
      Holder<T, elastica::states::tags::PrimitiveTag> const& l,
      Holder<T, elastica::states::tags::internal::
                    DerivativeMultipliedByTimeTag> const& r) {
    return l.t_ + r.t_;
  }

  template <typename T,
            Requires<not cpp17::is_same_v<T, CountedString>> = nullptr>
  Holder<T, elastica::states::tags::PrimitiveTag> operator+(
      Holder<T, elastica::states::tags::internal::
                    DerivativeMultipliedByTimeTag> const& r,
      Holder<T, elastica::states::tags::PrimitiveTag> const& l) {
    return l.t_ + r.t_;
  }

  template <typename T,
            Requires<not cpp17::is_same_v<T, CountedString>> = nullptr>
  Holder<T, elastica::states::tags::internal::DerivativeMultipliedByTimeTag>
  operator*(Holder<T, elastica::states::tags::DerivativeTag> const& l,
            TimeDelta const td) {
    return l.t_ * td.value();
  }

  struct CountedString : public counter<CountedString> {
    using p = counter<CountedString>;
    CountedString() : p(), s(){};
    CountedString(std::string str) : p(), s(std::move(str)){};
    CountedString(CountedString const& other) : p(other), s(other.s) {}
    CountedString(CountedString&& other) noexcept
        : p(std::move(other)), s(std::move(other.s)) {}

    //    // need a move assingment from a string to make it work
    //    CountedString& operator=(CountedString&& other) {
    //      // p = (std::move(other));
    //      s = std::move(other.s);
    //      return *this;
    //    }
    CountedString& operator=(std::string&& other) {
      s = std::move(other);
      return *this;
    }
    CountedString& operator=(std::string const& other) {
      s = other;
      return *this;
    }
    CountedString& operator=(CountedString const& other) {
      s = other.s;
      return *this;
    }

    std::string s;
  };

  void IGNORE_UNUSED reset_counter() {
    counter<CountedString>::objects_alive = 0;
    counter<CountedString>::objects_created = 0;
    counter<CountedString>::objects_copied = 0;
    counter<CountedString>::objects_moved = 0;
  }

  // this mimics an "optimized expression" that doesnt construct a
  // CountedString from scratch
  inline auto operator+(CountedString const& l, CountedString const& r)
      -> std::string {
    return l.s + r.s;
  }

  inline auto IGNORE_UNUSED operator+(CountedString const& l,
                                      std::string const& r) -> std::string {
    return l.s + r;
  }

  inline auto operator+(std::string const& l, CountedString const& r) {
    return l + r.s;
  }

  // I fucked up here. I added the tag to Holder after testing and this doesn't
  // seem to sit well with the expression template system. The intention here
  // anyway is not to test the tags themselves, but if the expressions are well
  // formed. So for now I allow conversion from Holder of any type to any other.
  // template <typename Tag1, typename Tag2>
  Holder<std::string, elastica::states::tags::PrimitiveTag> operator+(
      Holder<CountedString, elastica::states::tags::PrimitiveTag> const& l,
      Holder<CountedString, elastica::states::tags::internal::
                                DerivativeMultipliedByTimeTag> const& r) {
    return l.t_ + r.t_;
  }

  // template <typename Tag2>
  Holder<std::string, elastica::states::tags::PrimitiveTag> operator+(
      Holder<std::string, elastica::states::tags::PrimitiveTag> const& l,
      Holder<CountedString, elastica::states::tags::internal::
                                DerivativeMultipliedByTimeTag> const& r) {
    return l.t_ + r.t_;
  }

  // template <typename Tag1>
  Holder<std::string, elastica::states::tags::PrimitiveTag> operator+(
      Holder<CountedString, elastica::states::tags::internal::
                                DerivativeMultipliedByTimeTag> const& l,
      Holder<std::string, elastica::states::tags::PrimitiveTag> const& r) {
    // swithc order so that primitive is always first
    return r.t_ + l.t_;
  }

  /*
  Holder<std::string, elastica::states::tags::PrimitiveTag> operator+(
      Holder<CountedString, elastica::states::tags::PrimitiveTag> const& l,
      Holder<CountedString, elastica::states::tags::internal::
                                DerivativeMultipliedByTimeTag> const& r) {
    return l.t_ + r.t_;
  };

  Holder<std::string, elastica::states::tags::PrimitiveTag> operator+(
      Holder<std::string, elastica::states::tags::PrimitiveTag> const& l,
      Holder<CountedString, elastica::states::tags::internal::
                                DerivativeMultipliedByTimeTag> const& r) {
    return l.t_ + r.t_;
  };

  Holder<std::string,
         elastica::states::tags::internal::DerivativeMultipliedByTimeTag>
  operator+(Holder<CountedString, elastica::states::tags::internal::
                                      DerivativeMultipliedByTimeTag> const& l,
            Holder<CountedString, elastica::states::tags::internal::
                                      DerivativeMultipliedByTimeTag> const& r) {
    return l.t_ + r.t_;
  };

  Holder<std::string, elastica::states::tags::PrimitiveTag> operator+(
      Holder<std::string, elastica::states::tags::internal::
                              DerivativeMultipliedByTimeTag> const& l,
      Holder<CountedString, elastica::states::tags::internal::
                                DerivativeMultipliedByTimeTag> const& r) {
    return l.t_ + r.t_;
  };

  Holder<std::string, elastica::states::tags::PrimitiveTag> operator+(
      Holder<CountedString, elastica::states::tags::internal::
                                DerivativeMultipliedByTimeTag> const& l,
      Holder<std::string, elastica::states::tags::internal::
                              DerivativeMultipliedByTimeTag> const& r) {
    return l.t_ + r.t_;
  };
  */

}  // namespace

namespace {
  namespace {

    ELASTICA_TEST_CASE("Unit.Systems.States.State", "[Systems][Unit]") {
      using namespace elastica::states;

      int i = 2;
      std::string s("Hai");
      float f = 3.5F;
      // value semantics to be respected
      States<Holder<int>, Holder<std::string>, Holder<float>> state(i, s, f);
      // these are internal functions so a member is fine
      CHECK(state.get<0UL>().t_ == 2);
      CHECK(state.get<1UL>().t_ == "Hai");
      CHECK(state.get<2UL>().t_ == approx(3.5F));

      decltype(state) another_state(Holder<int>{3}, Holder<std::string>{"Bai"},
                                    Holder<float>{4.5F});
      CHECK(another_state.get<0UL>().t_ == 3);
      CHECK(another_state.get<1UL>().t_ == "Bai");
      CHECK(another_state.get<2UL>().t_ == approx(4.5F));
    }

  }  // namespace

  ELASTICA_TEST_CASE("Unit.Systems.States.StateExpressions",
                     "[Systems][Unit]") {
    using namespace elastica::states;

    int i = 2;
    std::string s("Hai");
    float f = 3.5F;
    // value semantics to be respected
    States<Holder<int>, Holder<std::string>, Holder<float>> state(i, s, f);
    // these are internal functions so a member is fine
    using A = Holder<int, tags::internal::DerivativeMultipliedByTimeTag>;
    using B =
        Holder<std::string, tags::internal::DerivativeMultipliedByTimeTag>;
    using C = Holder<float, tags::internal::DerivativeMultipliedByTimeTag>;
    States<A, B, C> another_state(A{3}, B{"Bai"}, C{4.5F});

    // check addition of two states
    {
      // this checks the + operator
      // and the get operator of state addition
      // and the assignment operator of state
      auto addexpr = state + another_state;
      static_assert(
          cpp17::is_same_v<
              decltype(addexpr),
              StatesStatesAddExpr<decltype(state), decltype(another_state)>>,
          "Failure!");

      // pipe it into a third state and see the results
      decltype(state) result_state;
      result_state = addexpr;

      CHECK(result_state.get<0UL>().t_ == 5);
      CHECK(result_state.get<1UL>().t_ == "HaiBai");
      CHECK(result_state.get<2UL>().t_ == approx(8.0F));
    }

    // check addition of three states
    {
      // this checks the + operator
      // and the get operator of state addition, recursively
      // and the assignment operator of state
      decltype(another_state) yet_another_state(A{4}, B{"Pai"}, C{5.0F});
      auto addexpr = state + another_state + yet_another_state;
      static_assert(
          cpp17::is_same_v<
              decltype(addexpr),
              StatesStatesAddExpr<
                  StatesStatesAddExpr<decltype(state), decltype(another_state)>,
                  decltype(yet_another_state)>>,
          "Failure!");
      // pipe it into a third state and see the results
      decltype(state) result_state;
      result_state = addexpr;

      CHECK(result_state.get<0UL>().t_ == 9);
      CHECK(result_state.get<1UL>().t_ == "HaiBaiPai");
      CHECK(result_state.get<2UL>().t_ == approx(13.0F));
    }

    // check addition of three states in a sequence
    {
      decltype(another_state) yet_another_state(A{4}, B{"Pai"}, C{5.0F});
      auto addexpr = state + another_state;
      auto another_addexpr = addexpr + yet_another_state;
      static_assert(
          cpp17::is_same_v<
              decltype(another_addexpr),
              StatesStatesAddExpr<
                  StatesStatesAddExpr<decltype(state), decltype(another_state)>,
                  decltype(yet_another_state)>>,
          "Failure!");

      auto yet_another_addexpr = yet_another_state + addexpr;
      static_assert(
          cpp17::is_same_v<decltype(yet_another_addexpr),
                           StatesStatesAddExpr<
                               decltype(yet_another_state),
                               StatesStatesAddExpr<decltype(state),
                                                   decltype(another_state)>>>,
          "Failure!");
      // pipe it into a third state and see the results
      decltype(state) result_state;
      result_state = another_addexpr;
      CHECK(result_state.get<0UL>().t_ == 9);
      CHECK(result_state.get<1UL>().t_ == "HaiBaiPai");
      CHECK(result_state.get<2UL>().t_ == approx(13.0F));

      result_state = yet_another_addexpr;

      CHECK(result_state.get<0UL>().t_ == 9);
      //! String addition is not commutative in general, but we defined the
      // +operators in Holder<> to respect the order
      CHECK(result_state.get<1UL>().t_ == "HaiBaiPai");
      CHECK(result_state.get<2UL>().t_ == approx(13.0F));
    }

    // check allocation of memory
    {
      using S = States<Holder<int>, Holder<CountedString>>;
      using TSA = Holder<
          int,
          ::elastica::states::tags::internal::DerivativeMultipliedByTimeTag>;
      using TSB = Holder<
          CountedString,
          ::elastica::states::tags::internal::DerivativeMultipliedByTimeTag>;
      using TS = States<TSA, TSB>;
      reset_counter();
      // here the constructor always copy constructs
      // but holder's variaid args constructor constructs it in place
      S counted_state(Holder<int>{2},
                      Holder<CountedString>{std::string("Hai")});
      // Holder<CountedString> = 1, plus a move into Holder's t_ = 2
      // Move it into S = 3 => 2 moves, 3 objects
      CHECK(counter<CountedString>::objects_created == 3);
      CHECK(counter<CountedString>::objects_alive == 1);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 2);

      TS another_counted_state(TSA{3}, TSB{std::string("Bai")});
      CHECK(counter<CountedString>::objects_created == 6);
      CHECK(counter<CountedString>::objects_alive == 2);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 4);

      TS yet_another_counted_state(TSA{4}, TSB{std::string("Pai")});
      CHECK(counter<CountedString>::objects_created == 9);
      CHECK(counter<CountedString>::objects_alive == 3);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 6);

      auto addexpr = counted_state + another_counted_state;
      CHECK(counter<CountedString>::objects_created == 9);
      CHECK(counter<CountedString>::objects_alive == 3);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 6);

      //      TypeDisplayer<typename decltype(addexpr)::ResultType> td{};

      auto another_add_expr = addexpr + yet_another_counted_state;
      CHECK(counter<CountedString>::objects_created == 9);
      CHECK(counter<CountedString>::objects_alive == 3);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 6);

      auto yet_another_addexpr = yet_another_counted_state + addexpr;
      CHECK(counter<CountedString>::objects_created == 9);
      CHECK(counter<CountedString>::objects_alive == 3);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 6);

      S result_state;
      // result state increments by 1
      CHECK(counter<CountedString>::objects_created == 10);
      CHECK(counter<CountedString>::objects_alive == 4);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 6);

      result_state = addexpr;
      CHECK(counter<CountedString>::objects_created == 10);
      CHECK(counter<CountedString>::objects_alive == 4);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 6);

      CHECK(result_state.get<0UL>().t_ == 5);
      CHECK(result_state.get<1UL>().t_.s == ("HaiBai"));

      result_state = another_add_expr;
      CHECK(counter<CountedString>::objects_created == 10);
      CHECK(counter<CountedString>::objects_alive == 4);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 6);
      CHECK(result_state.get<0UL>().t_ == 9);
      CHECK(result_state.get<1UL>().t_.s == ("HaiBaiPai"));

      result_state = yet_another_addexpr;
      CHECK(counter<CountedString>::objects_created == 10);
      CHECK(counter<CountedString>::objects_alive == 4);
      CHECK(counter<CountedString>::objects_copied == 0);
      CHECK(counter<CountedString>::objects_moved == 6);
      CHECK(result_state.get<0UL>().t_ == 9);
      CHECK(result_state.get<1UL>().t_.s == ("HaiBaiPai"));

      // reset_counter();
    }

    // check += expression
    {
      decltype(state) result_state(Holder<int>{4}, Holder<std::string>{"Pai"},
                                   Holder<float>{5.0F});
      result_state += another_state;

      CHECK(result_state.get<0UL>().t_ == 7);
      //! It gets switched as expected because string addition is not
      //! commutative
      CHECK(result_state.get<1UL>().t_ == "PaiBai");
      CHECK(result_state.get<2UL>().t_ == approx(9.5F));
    }

    // check mult expression with a TimeDelta
    {
      ::elastica::Time first(1.0), second(3.0);
      ::elastica::TimeDelta td(second - first);

      using S = States<Holder<int, ::elastica::states::tags::DerivativeTag>,
                       Holder<double, ::elastica::states::tags::DerivativeTag>>;
      // here the constructor always copy constructs
      // but holder's variaid args constructor constructs it in place
      S number_state(2, 3.0);
      auto mult_expr = number_state * td;
      static_assert(cpp17::is_same_v<decltype(mult_expr),
                                     StatesTimeDeltaMultExpr<S, TimeDelta>>,
                    "Failure!");
      S result_state;
      result_state = mult_expr;
      CHECK(result_state.get<0UL>().t_ == 4);
      //! It gets switched as expected because string addition is not
      //! commutative
      CHECK(result_state.get<1UL>().t_ == approx(6.0));

      auto another_mult_expr = td * number_state;
      static_assert(cpp17::is_same_v<decltype(another_mult_expr),
                                     StatesTimeDeltaMultExpr<S, TimeDelta>>,
                    "Failure!");
      S another_result_state;
      another_result_state = another_mult_expr;
      CHECK(another_result_state.get<0UL>().t_ == 4);
      //! It gets switched as expected because string addition is not
      //! commutative
      CHECK(another_result_state.get<1UL>().t_ == approx(6.0));

      auto yet_another_mult_expr = another_mult_expr * 5.0;
      static_assert(cpp17::is_same_v<decltype(yet_another_mult_expr),
                                     StatesTimeDeltaMultExpr<S, TimeDelta>>,
                    "Failure!");
      S yet_another_result_state;
      yet_another_result_state = yet_another_mult_expr;
      CHECK(yet_another_result_state.get<0UL>().t_ == 20);
      //! It gets switched as expected because string addition is not
      //! commutative
      CHECK(yet_another_result_state.get<1UL>().t_ == approx(30.0));

      auto yet_yet_another_mult_expr = 5.0 * another_mult_expr;
      static_assert(cpp17::is_same_v<decltype(yet_yet_another_mult_expr),
                                     StatesTimeDeltaMultExpr<S, TimeDelta>>,
                    "Failure!");
      S yet_yet_another_result_state;
      yet_yet_another_result_state = yet_yet_another_mult_expr;
      CHECK(yet_yet_another_result_state.get<0UL>().t_ == 20);
      //! It gets switched as expected because string addition is not
      //! commutative
      CHECK(yet_yet_another_result_state.get<1UL>().t_ == approx(30.0));
    }

    {
      ::elastica::Time first(1.0), second(3.0);
      ::elastica::TimeDelta td(second - first);

      using S = States<Holder<int, ::elastica::states::tags::DerivativeTag>,
                       Holder<double, ::elastica::states::tags::DerivativeTag>>;
      // here the constructor always copy constructs
      // but holder's variaid args constructor constructs it in place
      S number_state(2, 3.0);
      using TS = States<Holder<int, ::elastica::states::tags::PrimitiveTag>,
                        Holder<double, ::elastica::states::tags::PrimitiveTag>>;
      TS another_number_state(4, 5.0);
      auto mult_expr = number_state * td;
      auto added_expr = another_number_state + mult_expr;
      TS result_state;
      result_state = added_expr;

      CHECK(result_state.get<0UL>().t_ == (4 + 2 * 2));
      CHECK(result_state.get<1UL>().t_ == approx(5.0 + 3.0 * 2.0));
    }
  }

}  // namespace
