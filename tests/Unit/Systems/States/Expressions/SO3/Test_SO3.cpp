//
// Created by Tejaswin Parthasarathy on 5/26/21.
//
#include <type_traits>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Systems/States/Expressions/SO3.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/Types.hpp"
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

  // for SO3 assignment
  template <typename Type, typename AnotherType>
  auto SO3_primitive_assign(Holder<Type>& lhs, Holder<Type> const& rhs_matrix,
                            Holder<AnotherType> const& rhs_vector) noexcept
      -> void {
    // fake matrix multiplication algorithm
    lhs.t_ = rhs_matrix.t_ * (rhs_vector.t_) + rhs_vector.t_;
  }

  // for SO3 add assignment
  template <typename Type, typename AnotherType>
  auto SO3_primitive_add_assign(Holder<Type>& lhs,
                                Holder<AnotherType> const& rhs_vector) noexcept
      -> void {
    // fake in-place matrix multiplication algorithm
    lhs.t_ = (lhs.t_ * rhs_vector.t_) + rhs_vector.t_;
  }

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3", "[Systems][Unit]") {
    using namespace elastica::states;

    float f = 5.0F;
    using H = Holder<float>;
    H data(f);

    {
      using P = SO3<H, tags::PrimitiveTag>;
      P state(&data);
      // check if vectorized
      static_assert(::elastica::states::tt::is_vectorized_t<P>::value,
                    "SO3 meta failure");
      CHECK(std::addressof(data) == std::addressof(state.get()));
      CHECK(state.size() == 1UL);
    }
    {
      using D = SO3<H, tags::DerivativeTag>;
      D state(&data);
      // check if vectorized
      static_assert(::elastica::states::tt::is_vectorized_t<D>::value,
                    "SO3 meta failure");
      CHECK(std::addressof(data) == std::addressof(state.get()));
      CHECK(state.size() == 1UL);
    }
    {
      using DD = SO3<H, tags::DoubleDerivativeTag>;
      DD state(&data);
      // check if vectorized
      static_assert(::elastica::states::tt::is_vectorized_t<DD>::value,
                    "SO3 meta failure");
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
  //        T, std::void_t<std::invoke_result_t<T, Args...>>>
  //        : std::true_type {};
  //  };

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3TimeDeltaMultExpr",
                     "[Systems][Unit]") {
    using namespace elastica::states;

    using H = Holder<float>;
    {
      float new_f = 25.0F;
      H new_data(new_f);

      using P = SO3<H, tags::PrimitiveTag>;
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
      using D = SO3<H, tags::DerivativeTag>;
      D state(&data);

      auto expr = state * td;
      using E = decltype(expr);
      static_assert(
          std::is_same_v<E, SO3TimeDeltaMultExpr<D, ::elastica::TimeDelta>>,
          "Meta failure!");
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SO3 meta failure");
      static_assert(
          std::is_same_v<::elastica::states::tt::order_t<E>,
                           tags::internal::DerivativeMultipliedByTimeTag>,
          "SO3 meta failure");
      CHECK(float(expr.get()) == approx(10.0));
      CHECK(expr.size() == 1UL);

      // these check the methods from MultExprbase
      auto scaled_expr = 5.0 * expr;
      auto another_scaled_expr = expr * 5.0;
      auto yet_another_scaled_expr = 5.0 * td * state;
      auto yet_yet_another_scaled_expr = td * state * 5.0;

      using SE = decltype(scaled_expr);
      static_assert(
          std::is_same_v<SE, SO3TimeDeltaMultExpr<D, ::elastica::TimeDelta>>,
          "Meta failure!");
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SO3 meta failure");
      static_assert(
          std::is_same_v<::elastica::states::tt::order_t<E>,
                           tags::internal::DerivativeMultipliedByTimeTag>,
          "SO3 meta failure");
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
      using DD = SO3<H, tags::DoubleDerivativeTag>;
      DD state(&data);

      auto expr = state * td;
      using E = decltype(expr);
      static_assert(
          std::is_same_v<E, SO3TimeDeltaMultExpr<DD, ::elastica::TimeDelta>>,
          "Meta failure!");
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SO3 meta failure");
      static_assert(
          std::is_same_v<::elastica::states::tt::order_t<E>,
                           tags::internal::DoubleDerivativeMultipliedByTimeTag>,
          "SO3 meta failure");
      CHECK(float(expr.get()) == approx(10.0F));
      CHECK(expr.size() == 1UL);

      // these check the methods from MultExprbase
      auto scaled_expr = 5.0 * expr;
      auto another_scaled_expr = expr * 5.0;
      auto yet_another_scaled_expr = 5.0 * td * state;
      auto yet_yet_another_scaled_expr = td * state * 5.0;

      using SE = decltype(scaled_expr);
      static_assert(
          std::is_same_v<SE, SO3TimeDeltaMultExpr<DD, ::elastica::TimeDelta>>,
          "Meta failure!");
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SO3 meta failure");
      static_assert(
          std::is_same_v<::elastica::states::tt::order_t<E>,
                           tags::internal::DoubleDerivativeMultipliedByTimeTag>,
          "SO3 meta failure");
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
  ELASTICA_TEST_CASE("Unit.Systems.States.SO3SO3AddExpr", "[Systems][Unit]") {
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
      using S = SO3<H, Tag>;

      S state(&data);
      S another_state(&another_data);

      // //  error: no matching function for call to 'common_order_backend'
      // auto expr = state + another_state;
    });

    // check if derivative state can be added with one scaled float derivative
    {
      float f = 10.0F;
      H data(f);
      using D = SO3<H, tags::DerivativeTag>;
      D derivative(&data);

      float another_f = 20.0F;
      H another_data(another_f);
      using DD = SO3<H, tags::DoubleDerivativeTag>;
      DD double_derivative(&another_data);

      auto expr = derivative + double_derivative * td;
      static_assert(
          std::is_same_v<decltype(expr),
                           SO3SO3AddExpr<D, SO3TimeDeltaMultExpr<
                                                DD, ::elastica::TimeDelta>>>,
          "Meta failure!");
      CHECK(float(expr.get()) == approx(50.0F));
      CHECK(expr.size() == 1UL);

      // add a layer
      auto another_expr = expr + double_derivative * td;
      static_assert(
          std::is_same_v<
              decltype(another_expr),
              SO3SO3AddExpr<decltype(expr),
                            SO3TimeDeltaMultExpr<DD, ::elastica::TimeDelta>>>,
          "Meta failure!");
      CHECK(float(another_expr.get()) == approx(90.0F));
      CHECK(another_expr.size() == 1UL);

      // another layer
      auto yet_another_expr = double_derivative * td + expr;
      static_assert(
          std::is_same_v<
              SO3SO3AddExpr<SO3TimeDeltaMultExpr<DD, ::elastica::TimeDelta>,
                            decltype(expr)>,
              decltype(yet_another_expr)>,
          "Meta failure!");
      CHECK(float(yet_another_expr.get()) == approx(90.0F));
      CHECK(yet_another_expr.size() == 1UL);
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3SO3AddExpr_assign",
                     "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    ::elastica::Time first(1.0), second(1.1);
    ::elastica::TimeDelta td(second - first);

    float f = 5.0F;
    H data(f);
    using D = SO3<H, tags::DerivativeTag>;
    D derivative(&data);

    float another_f = 25.0F;
    H another_data(another_f);
    using DD = SO3<H, tags::DoubleDerivativeTag>;
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
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.SO3TimeDeltaMultExpr_derivative_add_assign",
      "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    ::elastica::Time first(1.0), second(1.1);
    ::elastica::TimeDelta td(second - first);

    float another_f = 25.0F;
    H another_data(another_f);
    using DD = SO3<H, tags::DoubleDerivativeTag>;
    DD double_derivative(&another_data);

    auto expr = td * double_derivative;

    float f = 5.0F;
    H result_h(f);
    using D = SO3<H, tags::DerivativeTag>;
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
  }

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3RotRotAddExpr",
                     "[Systems][Unit]") {
    using namespace elastica::states;

    float f = 5.0F;
    using H = Holder<float>;
    H data(f);
    using P = SO3<H, tags::PrimitiveTag>;
    P state(&data);

    float f2 = 10.0F;
    H data2(f2);
    using D = SO3<H, tags::DerivativeTag>;
    D derivative_state(&data2);

    ::elastica::Time first(1.0), second(3.0);
    ::elastica::TimeDelta td(second - first);

    auto expr = state + derivative_state * td;
    using E = decltype(expr);
    static_assert(
        std::is_same_v<
            E,
            SO3RotRotAddExpr<P, SO3TimeDeltaMultExpr<D, elastica::TimeDelta>>>,
        "Meta failure!");
    static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                  "SO3 meta failure");
    static_assert(std::is_same_v<::elastica::states::tt::order_t<E>,
                                   tags::PrimitiveTag>,
                  "SO3 meta failure");
    CHECK(expr.size() == 1UL);

    // explicitly banned for now
    //    auto another_expr = derivative_state * td + state;
    //    static_assert(
    //        std::is_same_v<
    //            decltype(another_expr),
    //            SO3RotRotAddExpr<P, SO3TimeDeltaMultExpr<D,
    //            elastica::TimeDelta>>>,
    //        "Meta failure!");
    //    static_assert(
    //        ::elastica::states::tt::is_vectorized_t<decltype(another_expr)>::value,
    //        "SO3 meta failure");
    //    static_assert(std::is_same_v<
    //                      ::elastica::states::tt::order_t<decltype(another_expr)>,
    //                      tags::PrimitiveTag>,
    //                  "SO3 meta failure");
    //    CHECK(expr.size() == 1UL);

    // add another layer

    float f3 = 5.0F;
    H another_data(f3);
    D another_derivative_state(&data2);

    auto yet_another_expr = expr + another_derivative_state * td;
    using F = decltype(yet_another_expr);
    static_assert(
        std::is_same_v<
            F,
            SO3RotRotAddExpr<
                P,
                SO3SO3AddExpr<SO3TimeDeltaMultExpr<D, elastica::TimeDelta>,
                              SO3TimeDeltaMultExpr<D, elastica::TimeDelta>>>>,
        "Meta failure!");
    static_assert(::elastica::states::tt::is_vectorized_t<F>::value,
                  "SO3 meta failure");
    static_assert(std::is_same_v<::elastica::states::tt::order_t<F>,
                                   tags::PrimitiveTag>,
                  "SO3 meta failure");
    CHECK(yet_another_expr.size() == 1UL);

    auto yet_yet_another_expr = expr + another_derivative_state * td;
    using G = decltype(yet_yet_another_expr);
    static_assert(
        std::is_same_v<
            G,
            SO3RotRotAddExpr<
                P,
                SO3SO3AddExpr<SO3TimeDeltaMultExpr<D, elastica::TimeDelta>,
                              SO3TimeDeltaMultExpr<D, elastica::TimeDelta>>>>,
        "Meta failure!");
    static_assert(::elastica::states::tt::is_vectorized_t<G>::value,
                  "SO3 meta failure");
    static_assert(std::is_same_v<::elastica::states::tt::order_t<G>,
                                   tags::PrimitiveTag>,
                  "SO3 meta failure");
    CHECK(yet_yet_another_expr.size() == 1UL);
  }

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3RotRotAddExpr_assign",
                     "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    using P = SO3<H, tags::PrimitiveTag>;
    using D = SO3<H, tags::DerivativeTag>;

    float f = 5.0F;
    H data(f);
    P state(&data);

    float f2 = 10.0F;
    H data2(f2);
    D derivative_state(&data2);

    ::elastica::Time first(1.0), second(3.0);
    ::elastica::TimeDelta td(second - first);

    auto expr = state + derivative_state * td;

    H result_h(0.0);
    D result(&result_h);
    // assignment
    result = expr;
    // lhs.t_ = rhs_matrix.t_ * (rhs_vector.t_) + rhs_vector.t_;
    // 5.0 * (10.0 * 2) + (10.0F * 2)
    CHECK(float(result.get()) == approx(120.0F));
    CHECK(float(result_h) == approx(120.0F));

    auto another_expr = expr + derivative_state * td * 4.0;
    result = another_expr;
    // lhs.t_ = rhs_matrix.t_ * (rhs_vector.t_) + rhs_vector.t_;
    // 5.0 * (10.0 * 2 * 5) + (10.0F * 2 * 5)
    // 500 + 100
    CHECK(float(result.get()) == approx(600.0F));
    CHECK(float(result_h) == approx(600.0F));
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.SO3TimeDeltaMultExpr_primitive_add_assign",
      "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    ::elastica::Time first(1.0), second(3.0);
    ::elastica::TimeDelta td(second - first);

    using P = SO3<H, tags::PrimitiveTag>;
    using D = SO3<H, tags::DerivativeTag>;

    float another_f = 10.0F;
    H another_data(another_f);
    D derivative(&another_data);

    auto expr = td * derivative;

    float f = 5.0F;
    H result_h(f);
    P result(&result_h);

    // assignment
    result += expr;

    // lhs.t_ = rhs_matrix.t_ * (rhs_vector.t_) + rhs_vector.t_;
    // 5.0 * (10.0 * 2) + (10.0F * 2)
    CHECK(float(result.get()) == approx(120.0F));
    CHECK(float(result_h) == approx(120.0F));

    auto another_expr = derivative * td;
    result += another_expr;
    // lhs.t_ = rhs_matrix.t_ * (rhs_vector.t_) + rhs_vector.t_;
    // 120.0 * (10.0 * 2) + (10.0F * 2)
    CHECK(float(result.get()) == approx(2420.0F));
    CHECK(float(result_h) == approx(2420.0F));

    // // Doesnt work, expected behavior
    // result += (result + double_derivative * td);
  }

}  // namespace