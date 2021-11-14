//
// Created by Tejaswin Parthasarathy on 5/28/21.
//
#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Systems/States/Expressions/SO3.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/Types.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/Cpp20.hpp"

// catch wants << declared and defined while comparing equality, so we include
// STD helpers
#include "Utilities/StdHelpers.hpp"

#if defined(ELASTICA_USE_BLAZE)
#include "Utilities/Math/BlazeDetail/BlazeRotation.hpp"
// this file is to be executed after Test_S03.cpp
namespace {
  template <typename BlazeStruct>
  auto to_vec(BlazeStruct const& blaze_struct) -> std::vector<float> {
    return std::vector<float>(blaze_struct.data(),
                              blaze_struct.data() + blaze_struct.capacity());
  }

  using ::elastica::Time;
  using ::elastica::TimeDelta;

  ELASTICA_TEST_CASE("Unit.Systems.States.SO3SO3AddExpr_assign_blaze",
                     "[Systems][Unit]") {
    using namespace elastica::states;
    using H = blaze::DynamicMatrix<float>;
    ::elastica::Time first(1.0), second(1.1);
    const ::elastica::TimeDelta td(second - first);

    const std::size_t dimensions = 3UL;
    const std::size_t size = 20UL;
    float f = 5.0F;
    H data(dimensions, size, f);
    using D = SO3<H, tags::DerivativeTag>;
    D derivative(&data);

    float another_f = 25.0F;
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
        cpp17::is_same_v<
            decltype(expr.get()),
            cpp20::remove_cvref_t<decltype(data + td.value() * another_data)>>,
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
    using H = blaze::DynamicMatrix<float>;
    ::elastica::Time first(1.0), second(1.1);
    const ::elastica::TimeDelta td(second - first);

    const std::size_t dimensions = 3UL;
    const std::size_t size = 20UL;

    float another_f = 25.0F;
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
        cpp17::is_same_v<
            decltype(expr.get()),
            cpp20::remove_cvref_t<decltype(td.value() * another_data)>>,
        "Vectorization failure");

    {
      // no resize version

      float f = 5.0F;
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

    using H_mat = blaze::DynamicTensor<float>;
    using H_SO3 = SO3<H_mat, tags::PrimitiveTag>;

    using DH_mat = blaze::DynamicMatrix<float>;
    using DH_SO3 = SO3<DH_mat, tags::DerivativeTag>;

    ::elastica::Time first(1.0), second(2.0);
    const ::elastica::TimeDelta td(second - first); // dt

    const std::size_t dimensions = 3UL;
    const std::size_t size = 1UL;

    //float f = 5.0F;
    //H_mat data(dimensions, dimensions, size, f); // Q
    H_mat data{{{1.0}, {0.0}, {0.0}},
               {{0.0}, {1.0}, {0.0}},
               {{0.0}, {0.0}, {1.0}}};
    H_SO3 primitive(&data);

    //float another_f = 25.0F;
    //DH_mat derivative_data(dimensions, size, another_f); // omega
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
      H_mat result_h(dimensions, dimensions, size);
      H_SO3 result(&result_h);
      H_mat expected_result1{{{0.254506},  {0.915374}, {-0.311957}},
                            {{-0.834834}, {0.370785}, {0.406903}},
                            {{0.488138},  {0.156873}, {0.858552}}};
      H_mat expected_result2{{{    -0.85169 },{    0.523437 },{   0.0252415 }},
                             {{    -0.32339 },{   -0.562872 },{    0.760654 }},
                             {{    0.412363 },{    0.639679 },{    0.648667 }}};
      H_mat expected_result3{{{ -0.641423 },{ -0.581572 },{  0.500351 } },
                             {{  0.758904 },{ -0.385401 },{  0.524911 } },
                             {{ -0.112438 },{  0.716408 },{  0.688562 } }};
      result = expr;

      // 25.0 * 0.1 + 5.0F
      CHECK_ITERABLE_APPROX(to_vec(result.get()),
                            to_vec(expected_result1));

      result = expr + 1.0 * (td * derivative);
      // 5.0F + 2.0 * 0.1 * 25.0 + 0.1 * 25.0 = 12.5F
      CHECK_ITERABLE_APPROX(to_vec(result.get()),
                            to_vec(expected_result2));

      result = expr + 2.0 * (td * derivative);
      // 5.0F + 2.0 * 0.1 * 25.0 + 0.1 * 25.0 = 12.5F
      CHECK_ITERABLE_APPROX(to_vec(result.get()),
                            to_vec(expected_result3));
    }

    {
      // TODO: resize version
      H_mat result_h;
      H_SO3 result(&result_h);

      // assignment, this needs to be resized
      result = expr;
      // 25.0 * 0.1 + 5.0F
      //CHECK_ITERABLE_APPROX(to_vec(result.get()),
      //                      to_vec(H_mat(dimensions, dimensions, size, 2.0F)));
    }
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.SO3TimeDeltaMultExpr_primitive_add_assign_blaze",
      "[Systems][Unit]") {
    using namespace elastica::states;
    ::elastica::Time first(1.0), second(2.0);
    const ::elastica::TimeDelta td(second - first);

    using H_mat = blaze::DynamicTensor<float>;
    using H_SO3 = SO3<H_mat, tags::PrimitiveTag>;
    using DH_mat = blaze::DynamicMatrix<float>;
    using DH_SO3 = SO3<DH_mat, tags::DerivativeTag>;

    //const std::size_t dimensions = 3UL;
    //const std::size_t size = 1UL;

    //float another_f = 25.0F;
    //DH_mat derivative_data(dimensions, size, another_f); // omega
    DH_mat derivative_data{{1.0}, {3.2}, {7.0}};
    derivative_data /= blaze::norm(derivative_data);
    derivative_data *= 76.0 * M_PI / 180.0;
    DH_SO3 derivative(&derivative_data);

    H_mat result_h{{{1.0}, {0.0}, {0.0}},
               {{0.0}, {1.0}, {0.0}},
               {{0.0}, {0.0}, {1.0}}};
    H_SO3 result(&result_h);

    H_mat expected_result1{{{ 0.254506}, {0.915374}, {-0.311957}},
                           {{-0.834834}, {0.370785}, { 0.406903}},
                           {{ 0.488138}, {0.156873}, { 0.858552}}};
    H_mat expected_result2{{{    -0.85169 },{    0.523437 },{   0.0252415 }},
                           {{    -0.32339 },{   -0.562872 },{    0.760654 }},
                           {{    0.412363 },{    0.639679 },{    0.648667 }}};
    H_mat expected_result3{{{ -0.641423 },{ -0.581572 },{  0.500351 } },
                           {{  0.758904 },{ -0.385401 },{  0.524911 } },
                           {{ -0.112438 },{  0.716408 },{  0.688562 } }};

    // assignment
    auto expr = td * derivative;
    result += expr;
    CHECK_ITERABLE_APPROX(to_vec(result.get()),
                          to_vec(expected_result1));

    auto another_expr = derivative * td;
    result += another_expr;
    CHECK_ITERABLE_APPROX(to_vec(result.get()),
                          to_vec(expected_result2));

    auto yet_another_expr = derivative * td;
    result += yet_another_expr;
    CHECK_ITERABLE_APPROX(to_vec(result.get()),
                          to_vec(expected_result3));
  }

}  // namespace
#endif  // ELASTICA_USE_BLAZE
