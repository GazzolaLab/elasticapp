//
// Created by Tejaswin Parthasarathy on 5/31/21.
//
#include <type_traits>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Systems/States/Expressions/SE3.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/Types.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
//#include "Utilities/TypeTraits/InvokeResult.hpp"

namespace {

  using ::elastica::Time;
  using ::elastica::TimeDelta;

  template <typename T>
  struct Holder {
    using ElementType = T;
    Holder(T t) : t_(t){};
    template <typename Arg>
    Holder& operator=(Holder<Arg> const& arg) {
      t_ = arg.t_;
      return *this;
    }
    explicit operator T() const noexcept { return t_; }

    template <typename Arg>
    Holder& operator+=(Holder<Arg> const& arg) {
      t_ += arg.t_;
      return *this;
    }

    T t_;
  };

  //  // for check comparisons
  //  bool operator==(Holder<float> const& l, Holder<float> const& r) {
  //    return l.t_ == approx(r.t_);
  //  }
  //  template <typename T, typename = typename std::enable_if<
  //                            std::is_constructible<float, T>::value>::type>
  //  bool operator==(Holder<float> const& l, T r) {
  //    return l.t_ == approx(r);
  //  }

  // for vectorizability
  template <typename T>
  Holder<T> operator+(Holder<T> const& l, Holder<T> const& r) {
    return l.t_ + r.t_;
  }

  template <typename T>
  Holder<T> operator*(Holder<T> const& l, float td) {
    return l.t_ * td;
  }

  template <typename T>
  Holder<T> operator*(float td, Holder<T> const& l) {
    return td * l.t_;
  }

  // for assignment operations
  template <typename Type>
  inline auto size_backend(Holder<Type> const&) noexcept -> std::size_t {
    return 1UL;
  }

  template <typename Type>
  inline auto resize_backend(Holder<Type>&, std::size_t) -> void{}

  ELASTICA_TEST_CASE("Unit.Systems.States.SE3", "[Systems][Unit]") {
    using namespace elastica::states;

    float f = 5.0F;
    using H = Holder<float>;
    H data(f);

    {
      using P = SE3<H, tags::PrimitiveTag>;
      P state(&data);
      // check if vectorized
      static_assert(::elastica::states::tt::is_vectorized_t<P>::value,
                    "SE3 meta failure");
      CHECK(std::addressof(data) == std::addressof(state.get()));
      CHECK(state.size() == 1UL);
    }
    {
      using D = SE3<H, tags::DerivativeTag>;
      D state(&data);
      // check if vectorized
      static_assert(::elastica::states::tt::is_vectorized_t<D>::value,
                    "SE3 meta failure");
      CHECK(std::addressof(data) == std::addressof(state.get()));
      CHECK(state.size() == 1UL);
    }
    {
      using DD = SE3<H, tags::DoubleDerivativeTag>;
      DD state(&data);
      // check if vectorized
      static_assert(::elastica::states::tt::is_vectorized_t<DD>::value,
                    "SE3 meta failure");
      CHECK(std::addressof(data) == std::addressof(state.get()));
      CHECK(state.size() == 1UL);
    }
  }

  //  template <typename... Args>
  //  struct WithArgs {
  //    template <typename T, typename = std::void_t<>>
  //    struct CanBeInstantiated : std::false_type {};
  //    template <typename T>
  //    struct CanBeInstantiated<
  //        T, std::void_t<std::tt::invoke_result_t<T, Args...>>>
  //        : std::true_type {};
  //  };

  ELASTICA_TEST_CASE("Unit.Systems.States.SE3TimeDeltaMultExpr",
                     "[Systems][Unit]") {
    using namespace elastica::states;

    using H = Holder<float>;
    {
      float new_f = 25.0F;
      H new_data(new_f);

      using P = SE3<H, tags::PrimitiveTag>;
      P new_state(&new_data);
      // this should not be well formed and was checked to be non-well formed
      // as promoting the order leads nowhere
      //      auto new_expr = new_state * td;
    }

    float f = 5.0F;
    H data(f);

    ::elastica::Time first(1.0), second(3.0);
    ::elastica::TimeDelta td(second - first);

    {
      using D = SE3<H, tags::DerivativeTag>;
      D state(&data);

      auto expr = state * td;
      using E = decltype(expr);
      static_assert(
          std::is_same_v<E, SE3TimeDeltaMultExpr<D, ::elastica::TimeDelta>>,
          "Meta failure!");
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SE3 meta failure");
      static_assert(
          std::is_same_v<::elastica::states::tt::order_t<E>,
                           tags::internal::DerivativeMultipliedByTimeTag>,
          "SE3 meta failure");
      CHECK(float(expr.get()) == approx(10.0));
      CHECK(expr.size() == 1UL);

      // these check the methods from MultExprbase
      auto scaled_expr = 5.0 * expr;
      auto another_scaled_expr = expr * 5.0;
      auto yet_another_scaled_expr = 5.0 * td * state;
      auto yet_yet_another_scaled_expr = td * state * 5.0;

      using SE = decltype(scaled_expr);
      static_assert(
          std::is_same_v<SE, SE3TimeDeltaMultExpr<D, ::elastica::TimeDelta>>,
          "Meta failure!");
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SE3 meta failure");
      static_assert(
          std::is_same_v<::elastica::states::tt::order_t<E>,
                           tags::internal::DerivativeMultipliedByTimeTag>,
          "SE3 meta failure");
      CHECK(float(scaled_expr.get()) == approx(50.0));
      CHECK(scaled_expr.size() == 1UL);

      static_assert(std::is_same_v<SE, decltype(another_scaled_expr)>,
                    "Meta failure!");
      static_assert(std::is_same_v<SE, decltype(yet_another_scaled_expr)>,
                    "Meta failure!");
      static_assert(std::is_same_v<SE, decltype(yet_yet_another_scaled_expr)>,
                    "Meta failure!");
      CHECK(float(another_scaled_expr.get()) == approx(50.0));
      CHECK(float(yet_another_scaled_expr.get()) == approx(50.0));
      CHECK(float(yet_yet_another_scaled_expr.get()) == approx(50.0));
      CHECK(another_scaled_expr.size() == 1UL);
      CHECK(yet_another_scaled_expr.size() == 1UL);
      CHECK(yet_yet_another_scaled_expr.size() == 1UL);
    }

    {
      using DD = SE3<H, tags::DoubleDerivativeTag>;
      DD state(&data);

      auto expr = state * td;
      using E = decltype(expr);
      static_assert(
          std::is_same_v<E, SE3TimeDeltaMultExpr<DD, ::elastica::TimeDelta>>,
          "Meta failure!");
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SE3 meta failure");
      static_assert(
          std::is_same_v<::elastica::states::tt::order_t<E>,
                           tags::internal::DoubleDerivativeMultipliedByTimeTag>,
          "SE3 meta failure");
      CHECK(float(expr.get()) == approx(10.0F));
      CHECK(expr.size() == 1UL);

      // these check the methods from MultExprbase
      auto scaled_expr = 5.0 * expr;
      auto another_scaled_expr = expr * 5.0;
      auto yet_another_scaled_expr = 5.0 * td * state;
      auto yet_yet_another_scaled_expr = td * state * 5.0;

      using SE = decltype(scaled_expr);
      static_assert(
          std::is_same_v<SE, SE3TimeDeltaMultExpr<DD, ::elastica::TimeDelta>>,
          "Meta failure!");
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SE3 meta failure");
      static_assert(
          std::is_same_v<::elastica::states::tt::order_t<E>,
                           tags::internal::DoubleDerivativeMultipliedByTimeTag>,
          "SE3 meta failure");
      CHECK(float(scaled_expr.get()) == approx(50.0F));
      CHECK(scaled_expr.size() == 1UL);

      static_assert(std::is_same_v<SE, decltype(another_scaled_expr)>,
                    "Meta failure!");
      static_assert(std::is_same_v<SE, decltype(yet_another_scaled_expr)>,
                    "Meta failure!");
      static_assert(std::is_same_v<SE, decltype(yet_yet_another_scaled_expr)>,
                    "Meta failure!");
      CHECK(float(another_scaled_expr.get()) == approx(50.0F));
      CHECK(float(yet_another_scaled_expr.get()) == approx(50.0F));
      CHECK(float(yet_yet_another_scaled_expr.get()) == approx(50.0F));
      CHECK(another_scaled_expr.size() == 1UL);
      CHECK(yet_another_scaled_expr.size() == 1UL);
      CHECK(yet_yet_another_scaled_expr.size() == 1UL);
    }
  }

  // can be tested after time-delta mult expr
  ELASTICA_TEST_CASE("Unit.Systems.States.SE3SE3AddExpr", "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    ::elastica::Time first(1.0), second(3.0);
    ::elastica::TimeDelta td(second - first);

    // first check if same states cannot be added with one
    // another
    tmpl::for_each<tmpl::list<tags::DoubleDerivativeTag>>([](auto t) {
      float f = 5.0F;
      H data(f);

      float another_f = 25.0F;
      H another_data(another_f);

      using Tag = tmpl::type_from<decltype(t)>;
      using S = SE3<H, Tag>;

      S state(&data);
      S another_state(&another_data);

      // //  error: no matching function for call to 'common_order_backend'
      // auto expr = state + another_state;
    });

    using opairs =
        tmpl::list<tmpl::pair<tags::PrimitiveTag, tags::DerivativeTag>,
                   tmpl::pair<tags::DerivativeTag, tags::DoubleDerivativeTag>>;

    // check if derivative state can be added with one scaled float derivative
    tmpl::for_each<opairs>([td](auto v) {
      using OPair = tmpl::type_from<decltype(v)>;
      using LowerOrderTag = typename OPair::first_type;
      using HigherOrderTag = typename OPair::second_type;

      float f = 10.0F;
      H data(f);
      using D = SE3<H, LowerOrderTag>;
      D derivative(&data);

      float another_f = 20.0F;
      H another_data(another_f);
      using DD = SE3<H, HigherOrderTag>;
      DD double_derivative(&another_data);

      auto expr = derivative + double_derivative * td;
      static_assert(
          std::is_same_v<decltype(expr),
                           SE3SE3AddExpr<D, SE3TimeDeltaMultExpr<
                                                DD, ::elastica::TimeDelta>>>,
          "Meta failure!");
      CHECK(float(expr.get()) == approx(50.0F));
      CHECK(expr.size() == 1UL);

      // add a layer
      auto another_expr = expr + double_derivative * td;
      static_assert(
          std::is_same_v<
              decltype(another_expr),
              SE3SE3AddExpr<decltype(expr),
                            SE3TimeDeltaMultExpr<DD, ::elastica::TimeDelta>>>,
          "Meta failure!");
      CHECK(float(another_expr.get()) == approx(90.0F));
      CHECK(another_expr.size() == 1UL);

      // another layer
      auto yet_another_expr = double_derivative * td + expr;
      static_assert(
          std::is_same_v<
              SE3SE3AddExpr<SE3TimeDeltaMultExpr<DD, ::elastica::TimeDelta>,
                            decltype(expr)>,
              decltype(yet_another_expr)>,
          "Meta failure!");
      CHECK(float(yet_another_expr.get()) == approx(90.0F));
      CHECK(yet_another_expr.size() == 1UL);
    });
  }

  ELASTICA_TEST_CASE("Unit.Systems.States.SE3SE3AddExpr_assign",
                     "[Systems][Unit]") {
    using namespace elastica::states;

    using opairs =
        tmpl::list<tmpl::pair<tags::PrimitiveTag, tags::DerivativeTag>,
                   tmpl::pair<tags::DerivativeTag, tags::DoubleDerivativeTag>>;

    tmpl::for_each<opairs>([](auto v) {
      using H = Holder<float>;
      ::elastica::Time first(1.0), second(1.1);
      ::elastica::TimeDelta td(second - first);

      using OPair = tmpl::type_from<decltype(v)>;
      using LowerOrderTag = typename OPair::first_type;
      using HigherOrderTag = typename OPair::second_type;

      float f = 5.0F;
      H data(f);
      using D = SE3<H, LowerOrderTag>;
      D derivative(&data);

      float another_f = 25.0F;
      H another_data(another_f);
      using DD = SE3<H, HigherOrderTag>;
      DD double_derivative(&another_data);

      auto expr = derivative + td * double_derivative;

      H result_h(0.0);
      D result(&result_h);
      // assignment
      result = expr;
      // 25.0 * 0.1 + 5.0F
      CHECK(float(result.get()) == approx(7.5F));
      CHECK(float(result_h) == approx(7.5F));

      result = expr + 2.0 * (td * double_derivative);
      // 5.0F + 2.0 * 0.1 * 25.0 + 0.1 * 25.0 = 12.5F
      CHECK(float(result_h) == approx(12.5F));

      // // this should not work and doesnt work
      // auto yet_another_expr = td * double_derivative;
      // result = yet_another_expr;
    });
  }

  ELASTICA_TEST_CASE("Unit.Systems.States.SE3TimeDeltaMultExpr_add_assign",
                     "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    using opairs =
        tmpl::list<tmpl::pair<tags::PrimitiveTag, tags::DerivativeTag>,
                   tmpl::pair<tags::DerivativeTag, tags::DoubleDerivativeTag>>;

    tmpl::for_each<opairs>([](auto v) {
      using OPair = tmpl::type_from<decltype(v)>;
      using LowerOrderTag = typename OPair::first_type;
      using HigherOrderTag = typename OPair::second_type;

      ::elastica::Time first(1.0), second(1.1);
      ::elastica::TimeDelta td(second - first);

      float another_f = 25.0F;
      H another_data(another_f);
      using DD = SE3<H, HigherOrderTag>;
      DD double_derivative(&another_data);

      auto expr = td * double_derivative;

      float f = 5.0F;
      H result_h(f);
      using D = SE3<H, LowerOrderTag>;
      D result(&result_h);
      // assignment
      result += expr;
      // 25.0 * 0.1 + 5.0F
      CHECK(float(result.get()) == approx(7.5F));
      CHECK(float(result_h) == approx(7.5F));

      auto another_expr = double_derivative * td;
      result += another_expr;
      // 25.0 * 0.1 + 7.5F
      CHECK(float(result.get()) == approx(10.0F));
      CHECK(float(result_h) == approx(10.0F));

      // Doesnt work, expected behavior
      // result += (result + double_derivative * td);
    });
  }

}  // namespace