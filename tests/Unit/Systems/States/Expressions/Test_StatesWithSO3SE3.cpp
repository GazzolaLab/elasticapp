//
// Created by Tejaswin Parthasarathy on 5/28/21.
//

#include <type_traits>

#include "Framework/TestingFramework.hpp"
#include "Systems/States/States.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/Types.hpp"

namespace {

  // copied from Test_SO3, can be refactored

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

  // for vectorizability inside SO3 and SE3
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

}  // namespace

namespace {

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.StatesWithSO3SE3.explicit_step_for_primitives",
      "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    using SO3P = SO3<H, tags::PrimitiveTag>;
    using SO3D = SO3<H, tags::DerivativeTag>;
    using SE3P = SE3<H, tags::PrimitiveTag>;
    using SE3D = SE3<H, tags::DerivativeTag>;
    using SP = States<SO3P, SE3P>;
    using SD = States<SO3D, SE3D>;

    ::elastica::Time first(1.0), second(3.0);
    const ::elastica::TimeDelta td(second - first);

    float derivative_f = 10.0F;
    H derivative_data(derivative_f);
    H se3_derivative_data(derivative_f);
    SD derivative{SO3D(&derivative_data), SE3D(&se3_derivative_data)};

    float another_derivative_f = 10.0F;
    H another_derivative_data(another_derivative_f);
    H se3_another_derivative_data(derivative_f);
    SD another_derivative{SO3D(&another_derivative_data),
                          SE3D(&se3_another_derivative_data)};

    float state_f = 5.0F;
    H state_data(state_f);
    H se3_state_data(state_f);
    SP state{SO3P(&state_data), SE3P(&se3_state_data)};

    auto expr = state + derivative * td;

    H result_state_data(0.0);
    H se3_result_state_data(0.0);
    SP result_state{SO3P(&result_state_data), SE3P(&se3_result_state_data)};

    // assignment
    result_state = expr;

    // check if results are updated
    CHECK(float(result_state_data) == approx(120.0F));
    // 2 * 10 + 5
    CHECK(float(se3_result_state_data) == approx(25.0F));

    auto another_expr = expr + another_derivative * td * 4.0;
    result_state = another_expr;
    // lhs.t_ = rhs_matrix.t_ * (rhs_vector.t_) + rhs_vector.t_;
    // 5.0 * (10.0 * 2 * 5) + (10.0F * 2 * 5)
    // 500 + 100
    CHECK(float(result_state_data) == approx(600.0F));
    // 5 * 2 * 10 + 5
    CHECK(float(se3_result_state_data) == approx(105.0F));
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.StatesWithSO3SE3.explicit_step_for_derivatives",
      "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    using SO3D = SO3<H, tags::DerivativeTag>;
    using SO3DD = SO3<H, tags::DoubleDerivativeTag>;
    using SE3D = SE3<H, tags::DerivativeTag>;
    using SE3DD = SE3<H, tags::DoubleDerivativeTag>;
    using SD = States<SO3D, SE3D>;
    using SDD = States<SO3DD, SE3DD>;

    ::elastica::Time first(1.0), second(1.1);
    const ::elastica::TimeDelta td(second - first);

    float derivative_f = 5.0F;
    H derivative_data(derivative_f);
    H se3_derivative_data(derivative_f);
    SD derivative{SO3D(&derivative_data), SE3D(&se3_derivative_data)};

    float double_derivative_f = 25.0F;
    H double_derivative_data(double_derivative_f);
    H se3_double_derivative_data(double_derivative_f);
    SDD double_derivative{SO3DD(&double_derivative_data),
                          SE3DD(&se3_double_derivative_data)};

    //    float another_double_derivative_f = 25.0F;
    //    H another_double_derivative_data(another_double_derivative_f);
    //    SDD another_double_derivative{D(&another_double_derivative_data)};

    auto expr = derivative + double_derivative * td;

    H result_derivative_data(0.0);
    H se3_result_derivative_data(0.0);
    SD result_derivative{SO3D(&result_derivative_data),
                         SE3D(&se3_result_derivative_data)};

    // assignment
    result_derivative = expr;

    // check if results are updated
    CHECK(float(result_derivative_data) == approx(7.5F));
    CHECK(float(se3_result_derivative_data) == approx(7.5F));

    result_derivative = expr + 2.0 * (td * double_derivative);
    // 5.0F + 2.0 * 0.1 * 25.0 + 0.1 * 25.0 = 12.5F
    CHECK(float(result_derivative_data) == approx(12.5F));
    CHECK(float(se3_result_derivative_data) == approx(12.5F));
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.StatesWithSO3SE3.symplectic_step_for_primitives",
      "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    using SO3P = SO3<H, tags::PrimitiveTag>;
    using SO3D = SO3<H, tags::DerivativeTag>;
    using SE3P = SE3<H, tags::PrimitiveTag>;
    using SE3D = SE3<H, tags::DerivativeTag>;
    using SP = States<SO3P, SE3P>;
    using SD = States<SO3D, SE3D>;

    ::elastica::Time first(1.0), second(3.0);
    const ::elastica::TimeDelta td(second - first);

    float derivative_f = 10.0F;
    H derivative_data(derivative_f);
    H se3_derivative_data(derivative_f);
    SD derivative{SO3D(&derivative_data), SE3D(&se3_derivative_data)};

    float another_derivative_f = 10.0F;
    H another_derivative_data(another_derivative_f);
    H se3_another_derivative_data(derivative_f);
    SD another_derivative{SO3D(&another_derivative_data),
                          SE3D(&se3_another_derivative_data)};

    auto expr = derivative * td;

    float result_state_f = 5.0F;
    H result_state_data(result_state_f);
    H se3_result_state_data(result_state_f);
    SP result_state{SO3P(&result_state_data), SE3P(&se3_result_state_data)};

    // assignment
    result_state += expr;

    // check if results are updated
    CHECK(float(result_state_data) == approx(120.0F));
    CHECK(float(se3_result_state_data) == approx(25.0F));

    auto another_expr = derivative * td;
    result_state += another_expr;
    // lhs.t_ = rhs_matrix.t_ * (rhs_vector.t_) + rhs_vector.t_;
    // 5.0 * (10.0 * 2 * 5) + (10.0F * 2 * 5)
    // 500 + 100
    CHECK(float(result_state_data) == approx(2420.0F));
    CHECK(float(se3_result_state_data) == approx(45.0F));
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.StatesWithSO3SE3.symplectic_step_for_derivatives",
      "[Systems][Unit]") {
    using namespace elastica::states;
    using H = Holder<float>;
    using SO3D = SO3<H, tags::DerivativeTag>;
    using SO3DD = SO3<H, tags::DoubleDerivativeTag>;
    using SE3D = SE3<H, tags::DerivativeTag>;
    using SE3DD = SE3<H, tags::DoubleDerivativeTag>;
    using SD = States<SO3D, SE3D>;
    using SDD = States<SO3DD, SE3DD>;

    ::elastica::Time first(1.0), second(1.1);
    const ::elastica::TimeDelta td(second - first);

    float double_derivative_f = 25.0F;
    H double_derivative_data(double_derivative_f);
    H se3_double_derivative_data(double_derivative_f);
    SDD double_derivative{SO3DD(&double_derivative_data),
                          SE3DD(&se3_double_derivative_data)};

    //    float another_double_derivative_f = 25.0F;
    //    H another_double_derivative_data(another_double_derivative_f);
    //    SDD another_double_derivative{D(&another_double_derivative_data)};

    auto expr = double_derivative * td;

    float result_derivative_f = 5.0F;
    H result_derivative_data(result_derivative_f);
    H se3_result_derivative_data(result_derivative_f);
    SD result_derivative{SO3D(&result_derivative_data),
                         SE3D(&se3_result_derivative_data)};

    // assignment
    result_derivative += expr;

    // check if results are updated
    CHECK(float(result_derivative_data) == approx(7.5F));
    CHECK(float(se3_result_derivative_data) == approx(7.5F));

    auto another_expr = td * double_derivative;
    result_derivative += another_expr;
    // 5.0F + 2.0 * 0.1 * 25.0 + 0.1 * 25.0 = 12.5F
    CHECK(float(result_derivative_data) == approx(10.0F));
    CHECK(float(se3_result_derivative_data) == approx(10.0F));
  }

}  // namespace
