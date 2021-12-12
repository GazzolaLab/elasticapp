//
// Created by Tejaswin Parthasarathy on 12/1/21.
//

#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
//
// needs to be included first
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/BaseTemplate.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/SIMD.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/Scalar.hpp"
//
#include "Systems/States/Expressions/SO3.hpp"
#include "Utilities/DefineTypes.h"

#if defined(ELASTICA_USE_BLAZE)

namespace {

  using ::elastica::real_t;

  template <typename BlazeStruct>
  auto to_vec(BlazeStruct const& blaze_struct) -> std::vector<real_t> {
    return std::vector<real_t>(blaze_struct.data(),
                               blaze_struct.data() + blaze_struct.capacity());
  }

  ELASTICA_TEST_CASE("Unit.Systems.States.add_assign_blaze_backends",
                     "[Systems][Unit]") {
    using namespace elastica::states;
    ::elastica::Time first(1.0), second(2.0);
    const ::elastica::TimeDelta td(second - first);

    using H_mat = blaze::DynamicTensor<real_t>;
    using DH_mat = blaze::DynamicMatrix<real_t>;

    // clang-format off
    H_mat expected_result1{{{ 0.254506}, {0.915374}, {-0.311957}},
                           {{-0.834834}, {0.370785}, { 0.406903}},
                           {{ 0.488138}, {0.156873}, { 0.858552}}};
    H_mat expected_result2{{{    -0.85169 },{    0.523437 },{   0.0252415 }},
                           {{    -0.32339 },{   -0.562872 },{    0.760654 }},
                           {{    0.412363 },{    0.639679 },{    0.648667 }}};
    H_mat expected_result3{{{ -0.641423 },{ -0.581572 },{  0.500351 } },
                           {{  0.758904 },{ -0.385401 },{  0.524911 } },
                           {{ -0.112438 },{  0.716408 },{  0.688562 } }};
    // clang-format on

    DH_mat derivative_data{{1.0}, {3.2}, {7.0}};
    derivative_data /= blaze::norm(derivative_data);
    derivative_data *= 76.0 * M_PI / 180.0;

    H_mat result_h{
        {{1.0}, {0.0}, {0.0}}, {{0.0}, {1.0}, {0.0}}, {{0.0}, {0.0}, {1.0}}};

    auto temp_to_synch(result_h);
    // The rotation computation cannot be computed asynchronously during
    // add_assign.
    auto rhs_vector = td.value() * derivative_data;

    SECTION("Add assign memory allocating version") {
      SO3_primitive_assign(result_h, temp_to_synch, rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(result_h), to_vec(expected_result1));

      auto second_to_sync(result_h);
      SO3_primitive_assign(result_h, second_to_sync, rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(result_h), to_vec(expected_result2));
    }

    SECTION("Add assign scalar version") {
      auto input(temp_to_synch);
      detail::SO3AddAssign<detail::SO3AddAssignKind::scalar>::apply(input,
                                                                    rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(input), to_vec(expected_result1));

      detail::SO3AddAssign<detail::SO3AddAssignKind::scalar>::apply(input,
                                                                    rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(input), to_vec(expected_result2));
    }

    // TODO : Fix issue #7, #37
    SECTION("Add assign SIMD version") {
      auto input(temp_to_synch);
      detail::SO3AddAssign<detail::SO3AddAssignKind::simd>::apply(input,
                                                                  rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(input), to_vec(expected_result1));

      detail::SO3AddAssign<detail::SO3AddAssignKind::simd>::apply(input,
                                                                  rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(input), to_vec(expected_result2));
    }
  }

}  // namespace

#endif  // ELASTICA_USE_BLAZE