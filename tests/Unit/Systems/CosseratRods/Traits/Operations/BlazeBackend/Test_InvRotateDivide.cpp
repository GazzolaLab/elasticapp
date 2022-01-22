//
// Created by Tejaswin Parthasarathy on 1/20/22.
//

#include <vector>

#include "Framework/TestHelpers.hpp"
#include "Framework/TestingFramework.hpp"
// needs to be included first
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/BaseTemplate.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/Blaze.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/SIMD.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/Scalar.hpp"
//
#include "Systems/CosseratRods/Traits/DataType/BlazeBackend/DataTypeTraits.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Geometry/AxisAngleRotation.hpp"

namespace {

  using ::elastica::real_t;
  using Scalar = ::elastica::cosserat_rod::VectorTag<real_t>;
  using Vector = ::elastica::cosserat_rod::MatrixTag<real_t>;
  using Matrix = ::elastica::cosserat_rod::TensorTag<real_t>;

  //  template <typename BlazeStruct>
  //  auto to_vec(BlazeStruct const& blaze_struct) -> std::vector<real_t> {
  //    return std::vector<real_t>(blaze_struct.data(),
  //                               blaze_struct.data() +
  //                               blaze_struct.capacity());
  //  }

  ELASTICA_TEST_CASE(
      "Unit.Systems.CosseratRods.Traits.Operations.inv_rotate_divide_backends",
      "[Systems][Unit]") {
    using namespace ::elastica::cosserat_rod;

    const std::size_t n_elems(32UL);
    constexpr std::size_t dimension(3UL);

    Matrix::data_type rot_matrix_batch(dimension, dimension, n_elems);
    Vector::data_type reference_rot_axis_vector_batch(dimension, n_elems - 1UL);
    Scalar::data_type normalize_vector_batch(n_elems - 1UL);

    {
      // setup
      normalize_vector_batch = 0.25;

      //      const Vector::data_type rot_matrix = {{0.254506, -0.834834,
      //      0.488138},
      //                                            {0.915374, 0.370785,
      //                                            0.156873},
      //                                            {-0.311957, 0.406903,
      //                                            0.858552}};
      //
      //      /* Transpose for similar reasons mentioned before */
      //      for (std::size_t i(0UL); i < dimension; ++i)
      //        for (std::size_t j(0UL); j < dimension; ++j)
      //          for (std::size_t k(0UL); k < n_elems; ++k)
      //            rot_matrix_batch(i, j, k) = rot_matrix(j, i);

      const ::elastica::Vec3 axis{1.0, 0.0, 1.0};
      for (std::size_t k(0UL); k < n_elems; ++k) {
        ::blaze::columnslice(rot_matrix_batch, k) =
            ::elastica::geometry::rotate_via_axis_angle(axis, k * M_PI / 20);
      }
    }

    //    SECTION("Blaze version") {
    //      using Op =
    //      detail::InvRotateDivideOp<detail::InvRotateDivideKind::blaze>;
    //      Op::apply(result_h, temp_to_synch, rhs_vector);
    //      CHECK_ITERABLE_APPROX(to_vec(result_h), to_vec(expected_result1));
    //
    //      auto second_to_sync(result_h);
    //      SO3_primitive_assign(result_h, second_to_sync, rhs_vector);
    //      CHECK_ITERABLE_APPROX(to_vec(result_h), to_vec(expected_result2));
    //    }

    using Op = detail::InvRotateDivideOp<detail::InvRotateDivideKind::blaze>;
    Op::apply(reference_rot_axis_vector_batch, rot_matrix_batch,
              normalize_vector_batch);

    SECTION("Scalar version") {
      using TestOp =
          detail::InvRotateDivideOp<detail::InvRotateDivideKind::scalar>;

      Vector::data_type test_rot_axis_vector_batch(dimension, n_elems - 1UL);

      TestOp::apply(test_rot_axis_vector_batch, rot_matrix_batch,
                    normalize_vector_batch);

      CHECK((test_rot_axis_vector_batch) == reference_rot_axis_vector_batch);
    }

    SECTION("SIMD version") {
      using TestOp =
          detail::InvRotateDivideOp<detail::InvRotateDivideKind::simd>;

      Vector::data_type test_rot_axis_vector_batch(dimension, n_elems - 1UL);

      TestOp::apply(test_rot_axis_vector_batch, rot_matrix_batch,
                    normalize_vector_batch);

      CHECK((test_rot_axis_vector_batch) == reference_rot_axis_vector_batch);
    }
  }
}  // namespace
