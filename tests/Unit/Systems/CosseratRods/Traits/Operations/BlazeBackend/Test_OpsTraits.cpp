//
// Created by Seung Hyun Kim on 10/3/21.
//

#include "Framework/TestingFramework.hpp"

#include "elastica/Systems/CosseratRods/Traits/DataType/BlazeBackend/DataTypeTraits.hpp"
// #include "elastica/Systems/CosseratRods/Traits/Operations/Protocols.hpp"
#include "elastica/Systems/CosseratRods/Traits/Operations/BlazeBackend/OpsTraits.hpp"
#include "elastica/Systems/CosseratRods/Traits/Operations/BlazeBackend/OptimizationLevel.hpp"

#include <blaze/Blaze.h>
#include <blaze_tensor/Blaze.h>

namespace {

  // Necessary dependencies (confidence dependencies only)
  using namespace elastica::cosserat_rod;
  using real_type = double;
  using Scalar = VectorTag<real_type>;
  using Vector = MatrixTag<real_type>;
  using Matrix = TensorTag<real_type>;
  constexpr std::size_t dimension(3UL);

  // Test Unit
  using Optimization = OptimizationLevel;
  using Operations = Operations<Optimization::basic>;

  // TODO: Protocol for Traits operations
  // static_assert(tt::assert_conforms_to<Operations, protocols::Operations>,
  //     "Protocol Failure!");

  //************************************************************************
  // Simple Operations
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: add",
                     "[System][Unit]") {
    int a = 5, b = 7;
    CHECK(Operations::add(a, b) == 12UL);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: expand_for_broadcast",
                     "[System][Unit]") {
    const std::size_t n_cols(10UL);
    const std::size_t n_rows(15UL);

    // Scalar expand to Vector
    Scalar::data_type scalar(n_rows);
    auto scalar_expand = Operations::expand_for_broadcast(scalar);
    CHECK(scalar_expand.columns() == 3UL);
    CHECK(scalar_expand.rows() == n_rows);

    // Vector expand to Matrix
    Vector::data_type vector(n_rows, n_cols);
    CHECK(vector.columns() == n_cols);
    CHECK(vector.rows() == n_rows);
    auto vector_expand = Operations::expand_for_broadcast(vector);
    CHECK(vector_expand.pages() == 3UL);
    CHECK(vector_expand.columns() == n_cols);
    CHECK(vector_expand.rows() == n_rows);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: inverse_cube",
                     "[System][Unit]") {
    CHECK(1); // TODO: non-priority test. It directly uses
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: transpose",
                     "[System][Unit]") {
    CHECK(1);
  }
  //************************************************************************
  // Batch Operations
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: batch_matvec",
                     "[System][Unit]") {
    for (std::size_t n_elems : {1, 32, 128}) {
      Matrix::data_type matrix_batch(dimension, dimension, n_elems);
      Vector::data_type vector_batch(dimension, n_elems),
          test_matvec_batch(dimension, n_elems),
          correct_matvec_batch(dimension, n_elems);
      Operations::batch_matvec(test_matvec_batch, matrix_batch, vector_batch);

      /* element-wise matvec product */
      for (std::size_t i(0UL); i < n_elems; ++i)
        blaze::column(correct_matvec_batch, i) =
            blaze::columnslice(matrix_batch, i) *
            blaze::column(vector_batch, i);

      CHECK(test_matvec_batch == correct_matvec_batch);
    }
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: batch_matmul",
                     "[System][Unit]") {
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: batch_cross",
                     "[System][Unit]") {
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: batch_dot",
                     "[System][Unit]") {
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: batch_norm",
                     "[System][Unit]") {
    CHECK(1);
  }
  //************************************************************************
  // Vector Operations
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: difference_kernel",
                     "[System][Unit]") {
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: two_point_difference_kkernel",
                     "[System][Unit]") {
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: average_kernel",
                     "[System][Unit]") {
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: quadrature_kernel",
                     "[System][Unit]") {
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: subtract_quadrature",
                     "[System][Unit]") {
    CHECK(1);
  }

  //************************************************************************
  // Rotation Operations
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: batch_inv_rotate",
                     "[System][Unit]") {
    CHECK(1);
  }
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: batch_inv_rotate_and_divide",
                     "[System][Unit]") {
    CHECK(1);
  }

  //************************************************************************
  // MiscOperations
  ELASTICA_TEST_CASE("Unit.System.CosseratRods.Traits.Operation: batch_subtrace_z_unit",
                     "[System][Unit]") {
    CHECK(1);
  }
} // NAMESPACE