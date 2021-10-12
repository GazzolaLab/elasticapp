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
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: add",
                     "[Systems][Unit]") {
    int a = 5, b = 7;
    CHECK(Operations::add(a, b) == 12UL);
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: expand_for_broadcast",
                     "[Systems][Unit]") {
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
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: inverse_cube",
                     "[Systems][Unit]") {
    CHECK(1); // TODO - non-priority: simple function from blaze
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: transpose",
                     "[Systems][Unit]") {
    CHECK(1); // TODO - non-priority: simple function from blaze
  }
  //************************************************************************
  // Batch Operations
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_matvec",
                     "[Systems][Unit]") {
    const std::size_t n_elems(64UL);
    Matrix::data_type matrix_batch(dimension, dimension, n_elems);
    Vector::data_type vector_batch(dimension, n_elems),
        test_matvec_batch(dimension, n_elems),
        correct_matvec_batch(dimension, n_elems);
    Operations::batch_matvec(test_matvec_batch, matrix_batch, vector_batch);

    /* element-wise matvec product */
    for (std::size_t i(0UL); i < n_elems; ++i)
      Vector::slice(correct_matvec_batch, i) =
          Matrix::slice(matrix_batch, i) * Vector::slice(vector_batch, i);

    CHECK(test_matvec_batch == correct_matvec_batch);
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_matmul",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_cross",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_dot",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_norm",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  //************************************************************************
  // Vector Operations
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: difference_kernel",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: two_point_difference_kkernel",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: average_kernel",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: quadrature_kernel",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: subtract_quadrature",
                     "[Systems][Unit]") {
    for (std::size_t n_elems : {1, 32, 128}) {
      // Define and Initialize
      const std::size_t n_nodes = n_elems + 1UL;

      // Define and initialize test vectors
      Vector::data_type input_vector_batch(dimension, n_elems),
                        test_vector_batch(dimension, n_nodes);
      test_vector_batch = real_type(5.0);
      input_vector_batch = real_type(1.0);

      // Define and initialize solution vectors
      Vector::data_type correct_vector_batch(dimension, n_nodes);
      correct_vector_batch = real_type(4.0);
      Vector::slice(correct_vector_batch, 0UL) = real_type(4.5);
      Vector::slice(correct_vector_batch, n_nodes - 1UL) = real_type(4.5);

      // Testing Operation
      Operations::subtract_quadrature(test_vector_batch, input_vector_batch);

      // Check
      CHECK(test_vector_batch == correct_vector_batch);
    }
  }

  //************************************************************************
  // Rotation Operations
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_inv_rotate",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_inv_rotate_and_divide",
                     "[Systems][Unit]") {
    for (std::size_t n_elems : {1, 32, 128}) {
      Matrix::data_type rot_matrix_batch(dimension, dimension, n_elems);
      Vector::data_type test_rot_axis_vector_batch(dimension, n_elems),
                        correct_rot_axis_vector_batch(dimension, n_elems),
                        normalize_vector_batch(dimension, n_elems);

      const real_type norm_factor(0.25);
      normalize_vector_batch = norm_factor;

      /* Previous correct matrix, which did not have a transpose */
      /* rot_matrix = [[0.254506, -0.834834, 0.488138], */
      /*            [0.915374, 0.370785, 0.156873], */
      /*            [-0.311957, 0.406903, 0.858552]] */
      const Vector::data_type rot_matrix = {
          {0.254506, -0.834834, 0.488138}, \
          {0.915374, 0.370785, 0.156873}, \
          {-0.311957, 0.406903, 0.858552} \
        };
      /* Transpose for similar reasons mentioned before */
      for (std::size_t i(0UL); i < dimension; ++i)
        for (std::size_t j(0UL); j < dimension; ++j)
          blaze::row(blaze::pageslice(rot_matrix_batch, i), j) =
              rot_matrix(j, i);
      Operations::batch_inv_rotate_and_divide(test_rot_axis_vector_batch, rot_matrix_batch, normalize_vector_batch);
      Scalar::data_type rot_axis_vector = {1.0, 3.2, 7.0};
      rot_axis_vector /= blaze::length(rot_axis_vector);
      const real_type theta = 76.0 * M_PI / 180.0;  // 76.0 degrees
      rot_axis_vector *= theta;
      for (std::size_t i(0UL); i < dimension; ++i)
        blaze::row(correct_rot_axis_vector_batch, i) = rot_axis_vector[i] / norm_factor;

      /* tolerance taken from pyelastica */
      /* TODO refactor 1e-6 */
      CHECK((test_rot_axis_vector_batch - correct_rot_axis_vector_batch) ==
            Approx(real_type(0.0)).margin(real_type(1e-6)));
    }
  }

  //************************************************************************
  // Misc Operations
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_subtrace_z_unit",
                     "[Systems][Unit]") {
    const std::size_t n_elems(5UL);

    // Initialize test vector
    Vector::data_type vec(dimension, n_elems);
    for (std::size_t row_idx = 0UL; row_idx < dimension; ++row_idx)
      for (std::size_t col_idx = 0UL; col_idx < n_elems; ++col_idx)
        vec(row_idx, col_idx) = static_cast<real_type>(row_idx + col_idx);

    // Test Operation
    CHECK(vec(2UL,1UL) == approx(3.0));
    CHECK(vec(2UL,3UL) == approx(5.0));

    Operations::batch_subtract_z_unit(vec);
    CHECK(vec(2UL,1UL) == approx(2.0));
    CHECK(vec(2UL,3UL) == approx(4.0));
  }
} // NAMESPACE