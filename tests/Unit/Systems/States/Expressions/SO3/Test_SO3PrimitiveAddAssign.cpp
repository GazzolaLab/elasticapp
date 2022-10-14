//
// Created by Tejaswin Parthasarathy on 12/1/21.
//

#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
//
// needs to be included first
#include "ModuleSettings/Vectorization.hpp"
//
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/BaseTemplate.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/SIMD.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAddAssign/Scalar.hpp"
//
#include "Systems/States/Expressions/SO3.hpp"
#include "Utilities/DefineTypes.h"

#if defined(ELASTICA_USE_BLAZE)

#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/views/Column.h>
#include <blaze/math/views/Submatrix.h>

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
    H_mat expected_result_1{
        {{0.25450619348149084}, {0.9153737692840798}, {-0.31195746502722094}},
        {{-0.8348342628404866}, {0.3707851059094285}, {0.40690313199004513}},
        {{0.488137635372581}, {0.15687341311510705}, {0.8585524918084395}}};
    H_mat expected_result_2{
        {{-0.8516901626985716}, {0.5234374213384307}, {0.025241487773656174}},
        {{-0.32338986831225147}, {-0.5628715080170252}, {0.7606540991381047}},
        {{0.41236253447111093}, {0.6396787720451501}, {0.6486664849977726}}};
    H_mat expected_result_3{
        {{-0.6414225949651109}, {-0.5815723164444999}, {0.5003505725125015}},
        {{0.7589035893644309}, {-0.38540056968696046}, {0.5249111762334062}},
        {{-0.11243841300015261}, {0.7164077342061106}, {0.6885619519343712}}};
    // clang-format on

    // Repeat it for at least 10-20 elements to really test SIMD version.
    const std::size_t n_elems = 21UL;  // dont make it multiple of 4
    H_mat expected_result1(3UL, 3UL, n_elems),
        expected_result2(3UL, 3UL, n_elems),
        expected_result3(3UL, 3UL, n_elems);

    for (auto idx = 0UL; idx < n_elems; ++idx) {
      blaze::columnslice(expected_result1, idx) =
          blaze::columnslice(expected_result_1, 0UL);
      blaze::columnslice(expected_result2, idx) =
          blaze::columnslice(expected_result_2, 0UL);
      blaze::columnslice(expected_result3, idx) =
          blaze::columnslice(expected_result_3, 0UL);
    }

    DH_mat _derivative_data{{1.0}, {3.2}, {7.0}};
    _derivative_data /= blaze::norm(_derivative_data);
    _derivative_data *= 76.0 * M_PI / 180.0;

    DH_mat derivative_data(3UL, n_elems);
    for (auto idx = 0UL; idx < n_elems; ++idx) {
      blaze::column(derivative_data, idx) =
          blaze::column(_derivative_data, 0UL);
    }

    H_mat _result_h{
        {{1.0}, {0.0}, {0.0}}, {{0.0}, {1.0}, {0.0}}, {{0.0}, {0.0}, {1.0}}};

    H_mat result_h(3UL, 3UL, n_elems);
    for (auto idx = 0UL; idx < n_elems; ++idx) {
      blaze::columnslice(result_h, idx) = blaze::columnslice(_result_h, 0UL);
    }

    auto temp_to_synch(result_h);
    // The rotation computation cannot be computed asynchronously during
    // add_assign.
    auto rhs_vector = td.value() * derivative_data;

    // TODO: approxmiation should be made in tighter precision
    SECTION("Add assign memory allocating version") {
      SO3_primitive_assign(result_h, temp_to_synch, rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(result_h), to_vec(expected_result1));

      auto second_to_sync(result_h);
      SO3_primitive_assign(result_h, second_to_sync, rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(result_h), to_vec(expected_result2));
    }

    SECTION("Add assign scalar version") {
      constexpr auto backend = detail::SO3AddAssignKind::scalar;

      auto input(temp_to_synch);
      detail::SO3AddAssign<backend>::apply(input, rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(input), to_vec(expected_result1));

      detail::SO3AddAssign<backend>::apply(input, rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(input), to_vec(expected_result2));
    }

    // TODO : Fix issue #7, #37
    SECTION("Add assign SIMD version") {
      auto input(temp_to_synch);
#if ELASTICA_USE_VECTORIZATION
      constexpr auto backend = detail::SO3AddAssignKind::simd;
#else
      constexpr auto backend = detail::SO3AddAssignKind::scalar;
#endif

      detail::SO3AddAssign<backend>::apply(input, rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(input), to_vec(expected_result1));

      detail::SO3AddAssign<backend>::apply(input, rhs_vector);
      CHECK_ITERABLE_APPROX(to_vec(input), to_vec(expected_result2));
    }
  }

}  // namespace

#endif  // ELASTICA_USE_BLAZE