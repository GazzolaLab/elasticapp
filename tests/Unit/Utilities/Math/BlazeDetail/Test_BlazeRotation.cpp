// #define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do
// this in one cpp file

#include "Framework/TestingFramework.hpp"
// #include "catch2/catch.hpp"

#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

#include "Utilities/Math/BlazeDetail/BlazeRotation.hpp"
#include "Utilities/Math/BlazeDetail/BlazeLinearAlgebra.hpp"
//#include "utils.h"

namespace {

  using ValueType = double;
  using VT = blaze::DynamicVector<ValueType, blaze::rowVector>;
  using MT = blaze::DynamicMatrix<ValueType, blaze::rowMajor>;
  using TT = blaze::DynamicTensor<ValueType>;
  constexpr std::size_t dimension(3UL);

  /* Following block tests rotation by theta (matrix exp ()) along X, Y, Z axes
   */
  /* and compares to rotation matrix forms along the given axes */
  ELASTICA_TEST_CASE(
      "Unit.Utilities.Math.BlazeDetail.BlazeRotation: Batch matrix exp() (vector(X, Y, Z axis) --> rotation matrix) test",
      "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      TT test_rot_matrix_batch(dimension, dimension, n_elems),
          correct_rot_matrix_batch(dimension, dimension, n_elems);
      MT rot_axis_vector_batch(dimension, n_elems);

      /* loop across X, Y, Z axes */
      for (std::size_t rot_axis_vector_dir : {0UL, 1UL, 2UL}) {
        rot_axis_vector_batch = ValueType(0.0);
        blaze::randomize(
            blaze::row(rot_axis_vector_batch, rot_axis_vector_dir));
        elastica::exp_batch(test_rot_matrix_batch, rot_axis_vector_batch);

        /* Notice that the correct_rot_mat seems to be a transpose */
        /* ie if you take a vector v, and do Rv, it seems to do a */
        /* rotation be -test_theta. */
        /* The catch in this case is that our directors (d_1, d_2, d_3) */
        /* are all aligned row-wise rather than columnwise. Thus we need */
        /* to multiply them by a R.transpose, which is equivalent to */
        /* multiplying by a R(-theta). */
        auto theta_batch =
            blaze::row(rot_axis_vector_batch, rot_axis_vector_dir);
        correct_rot_matrix_batch = ValueType(0.0);
        blaze::row(
            blaze::pageslice(correct_rot_matrix_batch, rot_axis_vector_dir),
            rot_axis_vector_dir) = ValueType(1.0);
        blaze::row(blaze::pageslice(correct_rot_matrix_batch,
                                    (rot_axis_vector_dir + 1UL) % dimension),
                   (rot_axis_vector_dir + 1UL) % dimension) =
            blaze::cos(theta_batch);
        blaze::row(blaze::pageslice(correct_rot_matrix_batch,
                                    (rot_axis_vector_dir + 1UL) % dimension),
                   (rot_axis_vector_dir + 2UL) % dimension) =
            blaze::sin(theta_batch);
        blaze::row(blaze::pageslice(correct_rot_matrix_batch,
                                    (rot_axis_vector_dir + 2UL) % dimension),
                   (rot_axis_vector_dir + 1UL) % dimension) =
            -blaze::sin(theta_batch);
        blaze::row(blaze::pageslice(correct_rot_matrix_batch,
                                    (rot_axis_vector_dir + 2UL) % dimension),
                   (rot_axis_vector_dir + 2UL) % dimension) =
            blaze::cos(theta_batch);

        /* CHECK(test_rot_matrix_batch == correct_rot_matrix_batch); */
        /* sadly the above one gives invalid operand error for tensors */
        for (std::size_t i(0UL); i < dimension; ++i)
          CHECK(blaze::pageslice(test_rot_matrix_batch, i) ==
                blaze::pageslice(correct_rot_matrix_batch, i));
      }
    }
  }

  /* Following block tests rotation by theta (matrix exp ()) for a canned */
  /* example for user convenience: Computer test code at page 5-7 from */
  /* "Rotation, Reflection, and Frame Changes Orthogonal tensors in */
  /* computational engineering  mechanics" by Rebecca Brennen, 2018, IOP */
  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeRotation: Batch matrix exp() (axis vector --> rotation matrix) user example",
                     "[Unit][Utilities]") {
    //! [exp_batch_example]
    const size_t n_elems = 1UL;
    TT expanded_test_rot_matrix(dimension, dimension, n_elems);
    MT expanded_rot_axis_vector{{1.0}, {3.2}, {7.0}};
    expanded_rot_axis_vector /= blaze::norm(expanded_rot_axis_vector);
    const ValueType theta = 76.0 * M_PI / 180.0;  // 76.0 degrees
    expanded_rot_axis_vector *= theta;
    elastica::exp_batch(expanded_test_rot_matrix,
                              expanded_rot_axis_vector);
    const TT expanded_correct_rot_matrix{{{0.254506}, {0.915374}, {-0.311957}},
                                         {{-0.834834}, {0.370785}, {0.406903}},
                                         {{0.488138}, {0.156873}, {0.858552}}};
    //! [exp_batch_example]

    for (std::size_t i(0UL); i < dimension; ++i)
      /* tolerance taken from pyelastica */
      /* TODO refactor 1e-6 */
      CHECK((blaze::pageslice(expanded_test_rot_matrix, i) -
             blaze::pageslice(expanded_correct_rot_matrix, i)) ==
            Approx(ValueType(0.0)).margin(ValueType(1e-6)));
  }

  /* Following block tests rotation by theta (matrix exp ()) for a canned */
  /* example: Computer test code at page 5-7 from "Rotation, Reflection, */
  /* and Frame Changes Orthogonal tensors in computational engineering */
  /* mechanics" by Rebecca Brennen, 2018, IOP */
  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeRotation: Batch matrix exp() (axis vector --> rotation matrix) canned test",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      TT test_rot_matrix_batch(dimension, dimension, n_elems),
          correct_rot_matrix_batch(dimension, dimension, n_elems);
      MT rot_axis_vector_batch(dimension, n_elems);
      VT rot_axis_vector = {1.0, 3.2, 7.0};
      rot_axis_vector /= blaze::length(rot_axis_vector);
      const ValueType theta = 76.0 * M_PI / 180.0;  // 76.0 degrees
      rot_axis_vector *= theta;
      for (std::size_t i(0UL); i < dimension; ++i)
        blaze::row(rot_axis_vector_batch, i) = rot_axis_vector[i];
      elastica::exp_batch(test_rot_matrix_batch, rot_axis_vector_batch);
      /* Previous correct matrix, which did not have a transpose */
      /* correct_rot_matrix = [[0.254506, -0.834834, 0.488138], */
      /*                       [0.915374, 0.370785, 0.156873], */
      /*                       [-0.311957, 0.406903, 0.858552]] */
      const MT correct_rot_matrix = {{0.254506, -0.834834, 0.488138},
                                     {0.915374, 0.370785, 0.156873},
                                     {-0.311957, 0.406903, 0.858552}};
      /* Transpose for similar reasons mentioned before */
      for (std::size_t i(0UL); i < dimension; ++i)
        for (std::size_t j(0UL); j < dimension; ++j)
          blaze::row(blaze::pageslice(correct_rot_matrix_batch, i), j) =
              correct_rot_matrix(j, i);

      for (std::size_t i(0UL); i < dimension; ++i)
        /* tolerance taken from pyelastica */
        /* TODO refactor 1e-6 */
        CHECK((blaze::pageslice(test_rot_matrix_batch, i) -
               blaze::pageslice(correct_rot_matrix_batch, i)) ==
              Approx(ValueType(0.0)).margin(ValueType(1e-6)));
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeRotation: Batch matrix exp() gives orthonormal rotation matrix) test",
            "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      TT rot_matrix_batch(dimension, dimension, n_elems),
          R_RT_batch(dimension, dimension, n_elems),
          RT_R_batch(dimension, dimension, n_elems),
          identity_batch(dimension, dimension, n_elems);
      MT rot_axis_vector_batch(dimension, n_elems);
      blaze::randomize(rot_axis_vector_batch);
      identity_batch = ValueType(0.0);
      for (std::size_t i(0UL); i < dimension; ++i)
        blaze::row(blaze::pageslice(identity_batch, i), i) = ValueType(1.0);
      elastica::exp_batch(rot_matrix_batch, rot_axis_vector_batch);
      elastica::batch_matmul(R_RT_batch, rot_matrix_batch,
                                blaze::trans(rot_matrix_batch, {1, 0, 2}));
      elastica::batch_matmul(RT_R_batch,
                                blaze::trans(rot_matrix_batch, {1, 0, 2}),
                                rot_matrix_batch);

      for (std::size_t i(0UL); i < dimension; ++i) {
        CHECK(blaze::pageslice(R_RT_batch, i) ==
              blaze::pageslice(identity_batch, i));
        CHECK(blaze::pageslice(RT_R_batch, i) ==
              blaze::pageslice(identity_batch, i));
      }
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeRotation: Batch matrix exp() gives unit determinant matrix) test",
            "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      TT rot_matrix_batch(dimension, dimension, n_elems),
          R_RT_batch(dimension, dimension, n_elems),
          RT_R_batch(dimension, dimension, n_elems);
      MT rot_axis_vector_batch(dimension, n_elems);
      VT test_det_batch(n_elems);
      blaze::randomize(rot_axis_vector_batch);
      elastica::exp_batch(rot_matrix_batch, rot_axis_vector_batch);
      /* for (std::size_t i(0UL); i < n_elems; ++i) */
      /*     test_det_batch[i] = */
      /*         blaze::det(blaze::columnslice(rot_matrix_batch, i)); */
      /* the above one did not work since blaze::det() gave */
      /* linker errors during compilation, maybe Cmake will make it work */
      test_det_batch = ValueType(0.0);
      for (std::size_t i(0UL); i < dimension; ++i)
        test_det_batch +=
            blaze::row(blaze::pageslice(rot_matrix_batch, 0UL), i) *
            (blaze::row(blaze::pageslice(rot_matrix_batch, 1UL),
                        (i + 1UL) % dimension) *
                 blaze::row(blaze::pageslice(rot_matrix_batch, 2UL),
                            (i + 2UL) % dimension) -
             blaze::row(blaze::pageslice(rot_matrix_batch, 1UL),
                        (i + 2UL) % dimension) *
                 blaze::row(blaze::pageslice(rot_matrix_batch, 2UL),
                            (i + 1UL) % dimension));

      CHECK(test_det_batch == ValueType(1.0));
    }
  }

  /* Following block tests inverse rotation by theta (matrix log ()) along X, */
  /* Y, Z axes by taking rotation matrix forms along the given axes and */
  /* comparing output axes */
  ELASTICA_TEST_CASE(
      "Unit.Utilities.Math.BlazeDetail.BlazeRotation: Batch matrix log() (rotation matrix --> vector(X, Y, Z axis)) test",
      "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      TT rot_matrix_batch(dimension, dimension, n_elems);
      MT test_rot_axis_vector_batch(dimension, n_elems),
          correct_rot_axis_vector_batch(dimension, n_elems);
      VT theta_batch(n_elems);
      blaze::randomize(theta_batch);

      /* loop across X, Y, Z axes */
      for (std::size_t rot_axis_vector_dir : {0UL, 1UL, 2UL}) {
        /* Notice that the correct_rot_mat seems to be a transpose */
        /* ie if you take a vector v, and do Rv, it seems to do a */
        /* rotation be -test_theta. */
        /* The catch in this case is that our directors (d_1, d_2, d_3) */
        /* are all aligned row-wise rather than columnwise. Thus we need */
        /* to multiply them by a R.transpose, which is equivalent to */
        /* multiplying by a R(-theta). */
        rot_matrix_batch = ValueType(0.0);
        blaze::row(blaze::pageslice(rot_matrix_batch, rot_axis_vector_dir),
                   rot_axis_vector_dir) = ValueType(1.0);
        blaze::row(blaze::pageslice(rot_matrix_batch,
                                    (rot_axis_vector_dir + 1UL) % dimension),
                   (rot_axis_vector_dir + 1UL) % dimension) =
            blaze::cos(theta_batch);
        blaze::row(blaze::pageslice(rot_matrix_batch,
                                    (rot_axis_vector_dir + 1UL) % dimension),
                   (rot_axis_vector_dir + 2UL) % dimension) =
            blaze::sin(theta_batch);
        blaze::row(blaze::pageslice(rot_matrix_batch,
                                    (rot_axis_vector_dir + 2UL) % dimension),
                   (rot_axis_vector_dir + 1UL) % dimension) =
            -blaze::sin(theta_batch);
        blaze::row(blaze::pageslice(rot_matrix_batch,
                                    (rot_axis_vector_dir + 2UL) % dimension),
                   (rot_axis_vector_dir + 2UL) % dimension) =
            blaze::cos(theta_batch);
        elastica::batch_inv_rotate(test_rot_axis_vector_batch, rot_matrix_batch);

        correct_rot_axis_vector_batch = ValueType(0.0);
        blaze::row(correct_rot_axis_vector_batch, rot_axis_vector_dir) =
            theta_batch;

        CHECK(test_rot_axis_vector_batch == correct_rot_axis_vector_batch);
      }
    }
  }

  /* Following block tests inverse rotation by theta (matrix log ()) for canned
   */
  /* example for user convenience: Computer test code at page 5-7 from */
  /* "Rotation, Reflection, and Frame Changes Orthogonal tensors in */
  /* computational engineering mechanics" by Rebecca Brennen, 2018, IOP */
  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeRotation: Batch matrix log() (rotation matrix --> axis vector) user example",
                        "[Unit][Utilities]") {
    //! [log_batch_example]
    const size_t n_elems = 1UL;
    const TT expanded_rot_matrix{{{0.254506}, {0.915374}, {-0.311957}},
                                 {{-0.834834}, {0.370785}, {0.406903}},
                                 {{0.488138}, {0.156873}, {0.858552}}};
    MT expanded_test_rot_axis_vector(dimension, n_elems);
    elastica::batch_inv_rotate(expanded_test_rot_axis_vector,
                              expanded_rot_matrix);
    MT expanded_correct_rot_axis_vector{{1.0}, {3.2}, {7.0}};
    expanded_correct_rot_axis_vector /=
        blaze::norm(expanded_correct_rot_axis_vector);
    const ValueType theta = 76.0 * M_PI / 180.0;  // 76.0 degrees
    expanded_correct_rot_axis_vector *= theta;
    //! [log_batch_example]

    /* tolerance taken from pyelastica */
    /* TODO refactor 1e-6 */
    CHECK((expanded_test_rot_axis_vector - expanded_correct_rot_axis_vector) ==
          Approx(ValueType(0.0)).margin(ValueType(1e-6)));
  }

  /* Following block tests inverse rotation by theta (matrix log ()) for canned*/
  /* example: Computer test code at page 5-7 from "Rotation, Reflection, */
  /* and Frame Changes Orthogonal tensors in computational engineering */
  /* mechanics" by Rebecca Brennen, 2018, IOP */
  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeRotation: Batch matrix log() (rotation matrix --> axis vector) canned test",
            "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      TT rot_matrix_batch(dimension, dimension, n_elems);
      MT test_rot_axis_vector_batch(dimension, n_elems),
          correct_rot_axis_vector_batch(dimension, n_elems);
      /* Previous correct matrix, which did not have a transpose */
      /* rot_matrix = [[0.254506, -0.834834, 0.488138], */
      /*            [0.915374, 0.370785, 0.156873], */
      /*            [-0.311957, 0.406903, 0.858552]] */
      const MT rot_matrix = {{0.254506, -0.834834, 0.488138},
                             {0.915374, 0.370785, 0.156873},
                             {-0.311957, 0.406903, 0.858552}};
      /* Transpose for similar reasons mentioned before */
      for (std::size_t i(0UL); i < dimension; ++i)
        for (std::size_t j(0UL); j < dimension; ++j)
          blaze::row(blaze::pageslice(rot_matrix_batch, i), j) =
              rot_matrix(j, i);
      elastica::batch_inv_rotate(test_rot_axis_vector_batch, rot_matrix_batch);
      VT rot_axis_vector = {1.0, 3.2, 7.0};
      rot_axis_vector /= blaze::length(rot_axis_vector);
      const ValueType theta = 76.0 * M_PI / 180.0;  // 76.0 degrees
      rot_axis_vector *= theta;
      for (std::size_t i(0UL); i < dimension; ++i)
        blaze::row(correct_rot_axis_vector_batch, i) = rot_axis_vector[i];

      /* tolerance taken from pyelastica */
      /* TODO refactor 1e-6 */
      CHECK((test_rot_axis_vector_batch - correct_rot_axis_vector_batch) ==
            Approx(ValueType(0.0)).margin(ValueType(1e-6)));
    }
  }
} // namespace