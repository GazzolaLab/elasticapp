//
// Created by Tejaswin Parthasarathy on 5/28/21.
//
#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
#include "Systems/States/Expressions/SE3.hpp"
#include "Time/SimulationTime.hpp"
#include "Time/Types.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

// catch wants << declared and defined while comparing equality, so we include
// STD helpers
#include "Utilities/StdHelpers.hpp"

#if defined(ELASTICA_USE_BLAZE)
// this file is to be executed after Test_S03.cpp
namespace {
  template <typename BlazeStruct>
  auto to_vec(BlazeStruct const& blaze_struct) -> std::vector<float> {
    return std::vector<float>(blaze_struct.data(),
                              blaze_struct.data() + blaze_struct.capacity());
  }

  using ::elastica::Time;
  using ::elastica::TimeDelta;

  ELASTICA_TEST_CASE("Unit.Systems.States.SE3SE3AddExpr_assign_blaze",
                     "[Systems][Unit]") {
    using namespace elastica::states;
    using H = blaze::DynamicMatrix<float>;
    ::elastica::Time first(1.0), second(1.1);
    const ::elastica::TimeDelta td(second - first);

    using opairs =
        tmpl::list<tmpl::pair<tags::PrimitiveTag, tags::DerivativeTag>,
                   tmpl::pair<tags::DerivativeTag, tags::DoubleDerivativeTag>>;

    // check if derivative state can be added with one scaled float derivative
    tmpl::for_each<opairs>([td](auto v) {
      using OPair = tmpl::type_from<decltype(v)>;
      using LowerOrderTag = typename OPair::first_type;
      using HigherOrderTag = typename OPair::second_type;

      const std::size_t dimensions = 3UL;
      const std::size_t size = 20UL;
      float f = 5.0F;
      H data(dimensions, size, f);
      using D = SE3<H, LowerOrderTag>;
      D derivative(&data);

      float another_f = 25.0F;
      H another_data(dimensions, size, another_f);
      using DD = SE3<H, HigherOrderTag>;
      DD double_derivative(&another_data);

      auto const expr = derivative + td * double_derivative;
      using E = decltype(expr);
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SE3 blaze meta failure");

      // RHS needs to be un-consted because thats the return type from blaze,
      // this usually shouldn't be a problem
      static_assert(
          std::is_same_v<decltype(expr.get()),
                           std::remove_cvref_t<
                               decltype(data + td.value() * another_data)>>,
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
    });
  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.States.SE3TimeDeltaMultExpr_derivative_add_assign_blaze",
      "[Systems][Unit]") {
    using namespace elastica::states;
    using H = blaze::DynamicMatrix<float>;
    ::elastica::Time first(1.0), second(1.1);
    const ::elastica::TimeDelta td(second - first);

    using opairs =
        tmpl::list<tmpl::pair<tags::PrimitiveTag, tags::DerivativeTag>,
                   tmpl::pair<tags::DerivativeTag, tags::DoubleDerivativeTag>>;

    // check if derivative state can be added with one scaled float derivative
    tmpl::for_each<opairs>([td](auto v) {
      using OPair = tmpl::type_from<decltype(v)>;
      using LowerOrderTag = typename OPair::first_type;
      using HigherOrderTag = typename OPair::second_type;

      const std::size_t dimensions = 3UL;
      const std::size_t size = 20UL;

      float another_f = 25.0F;
      H another_data(dimensions, size, another_f);
      using DD = SE3<H, HigherOrderTag>;
      DD double_derivative(&another_data);

      auto expr = td * double_derivative;
      using E = decltype(expr);
      static_assert(::elastica::states::tt::is_vectorized_t<E>::value,
                    "SE3 blaze meta failure");

      // RHS needs to be un-consted because thats the return type from blaze,
      // this usually shouldn't be a problem
      static_assert(
          std::is_same_v<
              decltype(expr.get()),
              std::remove_cvref_t<decltype(td.value() * another_data)>>,
          "Vectorization failure");

      {
        // no resize version

        float f = 5.0F;
        H result_h(dimensions, size, f);
        using D = SE3<H, LowerOrderTag>;
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
    });
  }

}  // namespace
#endif  // ELASTICA_USE_BLAZE
