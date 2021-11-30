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
  using Operations = OpsTraits<Optimization::basic>;

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
  //****************************************************************************
  // Batch Operations
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_matvec",
                     "[Systems][Unit]") {
    static Approx approx = Approx::custom().epsilon(1e-5);
    Matrix::data_type matrix_batch(3UL, 3UL, 9UL);
    Vector::data_type vector_batch(3UL, 9UL), test_matvec_batch(3UL, 9UL),
        correct_matvec_batch(3UL, 9UL);

    matrix_batch = {{{0.032009, 0.295688, 0.0802679, 0.444135, 0.848266,
                      0.947798, 0.0888524, 0.194528, 0.00564117},
                     {0.811298, 0.126109, 0.855166, 0.710564, 0.138555,
                      0.498771, 0.644454, 0.736467, 0.549105},
                     {0.607603, 0.847337, 0.122528, 0.149964, 0.968347,
                      0.376591, 0.124194, 0.439086, 0.0871375}},
                    {{0.56359, 0.417666, 0.33443, 0.683885, 0.168398, 0.752092,
                      0.353595, 0.832106, 0.781159},
                     {0.263162, 0.681189, 0.0652723, 0.544884, 0.240297,
                      0.616389, 0.649382, 0.39331, 0.648699},
                     {0.128448, 0.377084, 0.0678307, 0.830412, 0.0674234,
                      0.999041, 0.81527, 0.00714138, 0.0840087}},
                    {{0.768763, 0.478399, 0.362175, 0.0880274, 0.0254617,
                      0.356147, 0.0272758, 0.980988, 0.839524},
                     {0.530908, 0.0771562, 0.148289, 0.784449, 0.892867,
                      0.455831, 0.0608031, 0.894461, 0.305227},
                     {0.720157, 0.915282, 0.388605, 0.438926, 0.599945,
                      0.171136, 0.405663, 0.757049, 0.913819}}};
    vector_batch = {{0.614823, 0.611165, 0.470191, 0.278854, 0.2169, 0.825105,
                     0.304855, 0.746385, 0.989298},
                    {0.308332, 0.504112, 0.160027, 0.566804, 0.00126051,
                     0.407858, 0.733165, 0.614615, 0.915777},
                    {0.715569, 0.52956, 0.111037, 0.41087, 0.677285, 0.475944,
                     0.693132, 0.640913, 0.139238}};
    correct_matvec_batch = {{0.704611, 0.693003, 0.188196, 0.588215, 0.84001,
                             1.1647, 0.585661, 0.879252, 0.520571},
                            {0.519563, 0.798347, 0.175223, 0.840738, 0.0824933,
                             1.34744, 1.14899, 0.867383, 1.37856},
                            {1.15167, 0.815973, 0.237171, 0.649517, 0.412982,
                             0.561224, 0.334072, 1.76715, 1.2373}};

    Operations::batch_matvec(test_matvec_batch, matrix_batch, vector_batch);

    CHECK_MATRIX_CUSTOM_APPROX(test_matvec_batch, correct_matvec_batch, approx);
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

  //************************************************************************
  // Rotation Operations
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: batch_inv_rotate",
                     "[Systems][Unit]") {
    CHECK(1); // TODO: non-priority: linked to public utility
  }
  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operation: spanwise_inv_rotate_and_divide",
                     "[Systems][Unit]") {
    CHECK(1); // TODO Check math for spanwise inverse rotate
//    for (std::size_t n_elems : {1, 32, 128}) {
//      Matrix::data_type rot_matrix_batch(dimension, dimension, n_elems);
//      Vector::data_type test_rot_axis_vector_batch(dimension, n_elems),
//                        correct_rot_axis_vector_batch(dimension, n_elems);
//      Scalar::data_type normalize_vector_batch(dimension, n_elems);
//
//      const real_type norm_factor(0.25);
//      normalize_vector_batch = norm_factor;
//
//      /* Previous correct matrix, which did not have a transpose */
//      /* rot_matrix = [[0.254506, -0.834834, 0.488138], */
//      /*            [0.915374, 0.370785, 0.156873], */
//      /*            [-0.311957, 0.406903, 0.858552]] */
//      const Vector::data_type rot_matrix = {
//          {0.254506, -0.834834, 0.488138}, \
//          {0.915374, 0.370785, 0.156873}, \
//          {-0.311957, 0.406903, 0.858552} \
//        };
//      /* Transpose for similar reasons mentioned before */
//      for (std::size_t i(0UL); i < dimension; ++i)
//        for (std::size_t j(0UL); j < dimension; ++j)
//          blaze::row(blaze::pageslice(rot_matrix_batch, i), j) =
//              rot_matrix(j, i);
//      Operations::spanwise_inv_rotate_and_divide(
//          test_rot_axis_vector_batch, rot_matrix_batch, normalize_vector_batch);
//      Scalar::data_type rot_axis_vector = {1.0, 3.2, 7.0};
//      rot_axis_vector /= blaze::length(rot_axis_vector);
//      const real_type theta = 76.0 * M_PI / 180.0;  // 76.0 degrees
//      rot_axis_vector *= theta;
//      for (std::size_t i(0UL); i < dimension; ++i)
//        blaze::row(correct_rot_axis_vector_batch, i) = rot_axis_vector[i] / norm_factor;
//
//      /* tolerance taken from pyelastica */
//      /* TODO refactor 1e-6 */
//      CHECK((test_rot_axis_vector_batch - correct_rot_axis_vector_batch) ==
//            Approx(real_type(0.0)).margin(real_type(1e-6)));
//    }
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