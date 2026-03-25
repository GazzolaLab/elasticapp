//
// Created by Tejaswin Parthasarathy on 5/28/21.
//
#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Systems/States/Expressions/SO3.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/Types.hpp"
#include "Utilities/DefineTypes.h"

// catch wants << declared and defined while comparing equality, so we include
// STD helpers
#include "Utilities/StdHelpers.hpp"

#if defined(ELASTICA_USE_BLAZE)
#include "Utilities/Math/BlazeDetail/BlazeRotation.hpp"
// this file is to be executed after Test_S03.cpp
namespace {
  using ::elastica::real_t;

  template <typename BlazeStruct>
  auto to_vec(BlazeStruct const& blaze_struct) -> std::vector<real_t> {
    return std::vector<real_t>(blaze_struct.data(),
                               blaze_struct.data() + blaze_struct.capacity());
  }

  using ::elastica::Time;
  using ::elastica::TimeDelta;

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3SO3AddExpr_assign_blaze",
                     "[Systems][Unit]") {
    using namespace elastica::states;
    using H = blaze::DynamicMatrix<real_t>;
    ::elastica::Time first(1.0), second(1.1);
    const ::elastica::TimeDelta td(second - first);

    const std::size_t dimensions = 3UL;
    const std::size_t size = 20UL;
    real_t f = 5.0F;
    H data(dimensions, size, f);
    using D = SO3<H, tags::DerivativeTag>;
    D derivative(&data);

    real_t another_f = 25.0F;
    H another_data(dimensions, size, another_f);
    using DD = SO3<H, tags::DoubleDerivativeTag>;
    DD double_derivative(&another_data);

    auto const expr = derivative + td * double_derivative;
    using E = decltype(expr);
    static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                  "SO3 blaze meta failure");

    // RHS needs to be un-consted because thats the return type from blaze, this
    // usually shouldn't be a problem
    static_assert(
        std::is_same_v<
            decltype(expr.get()),
            std::remove_cvref_t<decltype(data + td.value() * another_data)>>,
        "Vectorization failure");

    {
      // no resize version
      H result_h(dimensions, size, 0.0);
      D result(&result_h);
      // assignment
      result = expr;
      // 25.0 * 0.1 + 5.0F
      CHECK_ITERABLE_APPROX(to_vec(result.get()),
                            to_vec(H(dimensions, size, 7.5F)));

      result = expr + 2.0 * (td * double_derivative);
      // 5.0F + 2.0 * 0.1 * 25.0 + 0.1 * 25.0 = 12.5F
      CHECK_ITERABLE_APPROX(to_vec(result.get()),
                            to_vec(H(dimensions, size, 12.5F)));
    }

    {
      // resize version
      H result_h;
      D result(&result_h);
      // assignment, this needs to be resized
      result = expr;
      // 25.0 * 0.1 + 5.0F
      CHECK_ITERABLE_APPROX(to_vec(result.get()),
                            to_vec(H(dimensions, size, 7.5F)));
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.SO3TimeDeltaMultExpr_derivative_add_assign_blaze",
      "[Systems][Unit]") {
    using namespace elastica::states;
    using H = blaze::DynamicMatrix<real_t>;
    ::elastica::Time first(1.0), second(1.1);
    const ::elastica::TimeDelta td(second - first);

    const std::size_t dimensions = 3UL;
    const std::size_t size = 20UL;

    real_t another_f = 25.0F;
    H another_data(dimensions, size, another_f);
    using DD = SO3<H, tags::DoubleDerivativeTag>;
    DD double_derivative(&another_data);

    auto expr = td * double_derivative;
    using E = decltype(expr);
    static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                  "SO3 blaze meta failure");

    // RHS needs to be un-consted because thats the return type from blaze, this
    // usually shouldn't be a problem
    static_assert(
        std::is_same_v<
            decltype(expr.get()),
            std::remove_cvref_t<decltype(td.value() * another_data)>>,
        "Vectorization failure");

    {
      // no resize version

      real_t f = 5.0F;
      H result_h(dimensions, size, f);
      using D = SO3<H, tags::DerivativeTag>;
      D result(&result_h);

      // assignment
      result += expr;
      // 25.0 * 0.1 + 5.0F
      CHECK_ITERABLE_APPROX(to_vec(result.get()),
                            to_vec(H(dimensions, size, 7.5F)));

      auto another_expr = double_derivative * td;
      result += another_expr;
      // 25.0 * 0.1 + 7.5F
      CHECK_ITERABLE_APPROX(to_vec(result.get()),
                            to_vec(H(dimensions, size, 10.0F)));
    }

    //    {
    //      // resize version, put extra size and take it all
    //      H result_h(dimensions, size + 4UL, f);
    //      using D = SO3<H, tags::DerivativeTag>;
    //      D result(&result_h);
    //
    //      // assignment
    //      result += expr;
    //      // 25.0 * 0.1 + 0.0F
    //      CHECK_ITERABLE_APPROX(to_vec(result.get()),
    //                            to_vec(H(dimensions, size, 2.5F)));
    //
    //      auto another_expr = double_derivative * td;
    //      result += another_expr;
    //      // 25.0 * 0.1 + 2.5F
    //      CHECK_ITERABLE_APPROX(to_vec(result.get()),
    //                            to_vec(H(dimensions, size, 5.0F)));
    //    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3RotRotAddExpr_assign_blaze",
                     "[Systems][Unit]") {
    using namespace elastica::states;

    using H_mat = blaze::DynamicTensor<real_t>;
    using H_SO3 = SO3<H_mat, tags::PrimitiveTag>;

    using DH_mat = blaze::DynamicMatrix<real_t>;
    using DH_SO3 = SO3<DH_mat, tags::DerivativeTag>;

    ::elastica::Time first(1.0), second(2.0);
    const ::elastica::TimeDelta td(second - first);  // dt

    const std::size_t dimensions = 3UL;
    const std::size_t size = 1UL;

    // H_mat data(dimensions, dimensions, size, f); // Q
    H_mat data{
        {{1.0}, {0.0}, {0.0}}, {{0.0}, {1.0}, {0.0}}, {{0.0}, {0.0}, {1.0}}};
    H_SO3 primitive(&data);

    // DH_mat derivative_data(dimensions, size, another_f); // omega
    DH_mat derivative_data{{1.0}, {3.2}, {7.0}};
    derivative_data /= blaze::norm(derivative_data);
    derivative_data *= 76.0 * M_PI / 180.0;
    DH_SO3 derivative(&derivative_data);

    auto const expr = primitive + td * derivative;
    using E = decltype(expr);
    static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                  "SO3 blaze meta failure");


    {
      // no resize version
      Approx approx =
          Approx::custom().epsilon(std::numeric_limits<double>::epsilon() * 10);
      H_mat result_h(dimensions, dimensions, size);
      H_SO3 result(&result_h);

      H_mat expected_result1{
          {{0.25450619348149084}, {0.9153737692840798}, {-0.31195746502722094}},
          {{-0.8348342628404866}, {0.3707851059094285}, {0.40690313199004513}},
          {{0.488137635372581}, {0.15687341311510705}, {0.8585524918084395}}};
      H_mat expected_result2{
          {{-0.8516901626985716}, {0.5234374213384307}, {0.025241487773656174}},
          {{-0.32338986831225147}, {-0.5628715080170252}, {0.7606540991381047}},
          {{0.41236253447111093}, {0.6396787720451501}, {0.6486664849977726}}};
      H_mat expected_result3{
          {{-0.6414225949651109}, {-0.5815723164444999}, {0.5003505725125015}},
          {{0.7589035893644309}, {-0.38540056968696046}, {0.5249111762334062}},
          {{-0.11243841300015261}, {0.7164077342061106}, {0.6885619519343712}}};

      // 25.0 * 0.1 + 5.0F
      result = expr;
      CHECK_ITERABLE_CUSTOM_APPROX(to_vec(result.get()),
                                   to_vec(expected_result1), approx);

      result = expr + 1.0 * (td * derivative);
      CHECK_ITERABLE_CUSTOM_APPROX(to_vec(result.get()),
                                   to_vec(expected_result2), approx);

      result = expr + 2.0 * (td * derivative);
      CHECK_ITERABLE_CUSTOM_APPROX(to_vec(result.get()),
                                   to_vec(expected_result3), approx);
    }

    {
      // TODO: resize version
      H_mat result_h;
      H_SO3 result(&result_h);

      // assignment, this needs to be resized
      result = expr;
      // 25.0 * 0.1 + 5.0F
      // CHECK_ITERABLE_APPROX(to_vec(result.get()),
      //                      to_vec(H_mat(dimensions, dimensions,
      //                      size, 2.0F)));
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.SO3TimeDeltaMultExpr_primitive_add_assign_blaze",
      "[Systems][Unit]") {
    using namespace elastica::states;
    ::elastica::Time first(1.0), second(2.0);
    const ::elastica::TimeDelta td(second - first);

    using H_mat = blaze::DynamicTensor<real_t>;
    using H_SO3 = SO3<H_mat, tags::PrimitiveTag>;
    using DH_mat = blaze::DynamicMatrix<real_t>;
    using DH_SO3 = SO3<DH_mat, tags::DerivativeTag>;

    Approx approx = Approx::custom()
                        .epsilon(std::numeric_limits<double>::epsilon() * 100)
                        .margin(1.0e-12);

    // DH_mat derivative_data(dimensions, size, another_f); // omega
    DH_mat derivative_data{{1.0}, {3.2}, {7.0}};
    derivative_data /= blaze::norm(derivative_data);
    derivative_data *= 76.0 * M_PI / 180.0;
    DH_SO3 derivative(&derivative_data);

    H_mat result_h1{
        {{1.0}, {0.0}, {0.0}}, {{0.0}, {1.0}, {0.0}}, {{0.0}, {0.0}, {1.0}}};
    H_mat result_h2{
        {{1.0}, {0.0}, {0.0}}, {{0.0}, {1.0}, {0.0}}, {{0.0}, {0.0}, {1.0}}};
    H_mat result_h3{
        {{1.0}, {0.0}, {0.0}}, {{0.0}, {1.0}, {0.0}}, {{0.0}, {0.0}, {1.0}}};
    H_SO3 result1(&result_h1);
    H_SO3 result2(&result_h2);
    H_SO3 result3(&result_h3);

    H_mat expected_result1{
        {{0.25450619348149084}, {0.9153737692840798}, {-0.31195746502722094}},
        {{-0.8348342628404866}, {0.3707851059094285}, {0.40690313199004513}},
        {{0.488137635372581}, {0.15687341311510705}, {0.8585524918084395}}};
    H_mat expected_result2{
        {{-0.8516901626985716}, {0.5234374213384307}, {0.025241487773656174}},
        {{-0.32338986831225147}, {-0.5628715080170252}, {0.7606540991381047}},
        {{0.41236253447111093}, {0.6396787720451501}, {0.6486664849977726}}};
    H_mat expected_result3{
        {{-0.6414225949651109}, {-0.5815723164444999}, {0.5003505725125015}},
        {{0.7589035893644309}, {-0.38540056968696046}, {0.5249111762334062}},
        {{-0.11243841300015261}, {0.7164077342061106}, {0.6885619519343712}}};

    // assignment
    auto expr = td * derivative;
    result1 += expr;
    CHECK_ITERABLE_CUSTOM_APPROX(to_vec(result1.get()),
                                 to_vec(expected_result1), approx);

    auto another_expr = 2.0 * (derivative * td);
    result2 += another_expr;
    CHECK_ITERABLE_CUSTOM_APPROX(to_vec(result2.get()),
                                 to_vec(expected_result2), approx);

    auto yet_another_expr = 3.0 * (derivative * td);
    result3 += yet_another_expr;
    CHECK_ITERABLE_CUSTOM_APPROX(to_vec(result3.get()),
                                 to_vec(expected_result3), approx);
  }

}  // namespace
#endif  // ELASTICA_USE_BLAZE
