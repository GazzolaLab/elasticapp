//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do
// this in one cpp file

#include "Framework/TestingFramework.hpp"
// #include "catch2/catch.hpp"

#include <cmath>

#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

#include "Utilities/Math/BlazeDetail/BlazeLinearAlgebra.hpp"

namespace {

  using ValueType = double;
  using VT = blaze::DynamicVector<ValueType>; //, blaze::rowVector>;
  using MT = blaze::DynamicMatrix<ValueType>;
  using TT = blaze::DynamicTensor<ValueType>;
  constexpr std::size_t dimension(3UL);

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Randomized batch matrix-vector product test",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {32UL, 64UL}) {
      TT matrix_batch(dimension, dimension, n_elems);
      MT vector_batch(dimension, n_elems),
          test_matvec_batch(dimension, n_elems),
          correct_matvec_batch(dimension, n_elems);
      blaze::randomize(matrix_batch);
      blaze::randomize(vector_batch);
      elastica::batch_matvec(test_matvec_batch, matrix_batch, vector_batch);

      /* element-wise matvec product */
      for (std::size_t i(0UL); i < n_elems; ++i)
        blaze::column(correct_matvec_batch, i) =
            blaze::columnslice(matrix_batch, i) *
            blaze::column(vector_batch, i);

      CHECK(test_matvec_batch == correct_matvec_batch);
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Analytical batch matrix-vector product test",
            "[Unit][Utilities]") {
    //! [batch_matvec_example]
    const size_t n_elems = 1UL;
    const MT expanded_vector{{1.0}, {2.0}, {3.0}};
    const TT expanded_matrix{{{1.0}, {2.0}, {3.0}},
                             {{4.0}, {5.0}, {6.0}},
                             {{7.0}, {8.0}, {9.0}}};  // row-slice gives page-wise direction vector 1,4,7
    MT test_matvec(dimension, n_elems);
    auto matmul_solution = blaze::columnslice(expanded_matrix, 0) * blaze::column(expanded_vector, 0);
    elastica::batch_matvec(test_matvec, expanded_matrix, expanded_vector);
    const MT correct_matvec{{14.0}, {32.0}, {50.0}};
    //! [batch_matvec_example]

    CHECK(test_matvec == correct_matvec);
    CHECK(matmul_solution == blaze::column(correct_matvec, 0));
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Randomised batch matrix-matrix product test",
            "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      TT first_matrix_batch(dimension, dimension, n_elems),
          second_matrix_batch(dimension, dimension, n_elems),
          test_matmul_batch(dimension, dimension, n_elems),
          correct_matmul_batch(dimension, dimension, n_elems);
      blaze::randomize(first_matrix_batch);
      blaze::randomize(second_matrix_batch);
      elastica::batch_matmul(test_matmul_batch, first_matrix_batch,
                                second_matrix_batch);

      /* element-wise matmul product */
      for (std::size_t i(0UL); i < n_elems; ++i)
        blaze::columnslice(correct_matmul_batch, i) =
            blaze::columnslice(first_matrix_batch, i) *
            blaze::columnslice(second_matrix_batch, i);

      CHECK(test_matmul_batch == correct_matmul_batch);
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Analytical batch matrix-matrix product test",
            "[Unit][Utilities]") {
    //! [batch_matmul_example]
    const size_t n_elems = 1UL;
    const TT expanded_first_matrix{{{1.0}, {2.0}, {3.0}},
                                   {{4.0}, {5.0}, {6.0}},
                                   {{7.0}, {8.0}, {9.0}}};  //
    const TT expanded_second_matrix{{{10.0}, {11.0}, {12.0}},
                                    {{13.0}, {14.0}, {15.0}},
                                    {{16.0}, {17.0}, {18.0}}};
    TT test_matmul(dimension, dimension, n_elems);
    elastica::batch_matmul(test_matmul, expanded_first_matrix,
                              expanded_second_matrix);
    const TT correct_matmul{{{84.0}, {90.0}, {96.0}},
                            {{201.0}, {216.0}, {231.0}},
                            {{318.0}, {342.0}, {366.0}}};
    //! [batch_matmul_example]

    CHECK(test_matmul == correct_matmul);
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Randomised batch cross product test",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      MT first_vector_batch(dimension, n_elems),
          second_vector_batch(dimension, n_elems),
          test_cross_product_batch(dimension, n_elems),
          correct_cross_product_batch(dimension, n_elems);
      blaze::randomize(first_vector_batch);
      blaze::randomize(second_vector_batch);
      elastica::batch_cross(test_cross_product_batch, first_vector_batch,
                               second_vector_batch);

      /* element-wise cross product_batch */
      for (std::size_t i(0UL); i < n_elems; ++i)
        blaze::column(correct_cross_product_batch, i) =
            blaze::cross(blaze::column(first_vector_batch, i),
                         blaze::column(second_vector_batch, i));

      CHECK(test_cross_product_batch == correct_cross_product_batch);
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Analytical batch cross product test",
                     "[Unit][Utilities]") {
    //! [batch_cross_example]
    const size_t n_elems = 1UL;
    const MT expanded_first_vector{{1.0}, {2.0}, {3.0}};
    const MT expanded_second_vector{{4.0}, {5.0}, {6.0}};
    MT test_cross_product(dimension, n_elems);
    elastica::batch_cross(test_cross_product, expanded_first_vector,
                             expanded_second_vector);
    const MT correct_cross_product{{-3.0}, {6.0}, {-3.0}};
    //! [batch_cross_example]

    CHECK(test_cross_product == correct_cross_product);
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Randomised batch dot product test",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      MT first_vector_batch(dimension, n_elems),
          second_vector_batch(dimension, n_elems);
      VT test_dot_product_batch(n_elems), correct_dot_product_batch(n_elems);
      blaze::randomize(first_vector_batch);
      blaze::randomize(second_vector_batch);
      test_dot_product_batch = elastica::batch_dot(first_vector_batch, second_vector_batch);

      /* element-wise dot product_batch */
      for (std::size_t i(0UL); i < n_elems; ++i)
        correct_dot_product_batch[i] =
            blaze::dot(blaze::column(first_vector_batch, i),
                       blaze::column(second_vector_batch, i));

      CHECK(test_dot_product_batch == correct_dot_product_batch);
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Analytical batch dot product test",
                     "[Unit][Utilities]") {
    //! [batch_dot_example]
    const size_t n_elems = 1UL;
    const MT expanded_first_vector{{1.0}, {2.0}, {3.0}};
    const MT expanded_second_vector{{4.0}, {5.0}, {6.0}};
    VT test_dot_product(n_elems);
    test_dot_product =
        elastica::batch_dot(expanded_first_vector, expanded_second_vector);
    VT correct_dot_product{32.0};
    //! [batch_dot_example]

    CHECK(test_dot_product == correct_dot_product);
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Randomised batch norm test",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {1UL, 32UL, 128UL}) {
      MT vector_batch(dimension, n_elems);
      VT test_norm_batch(n_elems), correct_norm_batch(n_elems);
      blaze::randomize(vector_batch);
      test_norm_batch = elastica::batch_norm(vector_batch);

      /* element-wise norm_batch*/
      for (std::size_t i(0UL); i < n_elems; ++i)
        correct_norm_batch[i] = std::sqrt(
            blaze::sum(blaze::pow(blaze::column(vector_batch, i), 2)));

      CHECK(test_norm_batch == correct_norm_batch);
    }
  }

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeLinearAlgebra: Analytical batch norm test",
                     "[Unit][Utilities]") {
    //! [batch_norm_example]
    const size_t n_elems = 1UL;
    const MT expanded_vector{{1.0}, {2.0}, {3.0}};
    VT test_norm(n_elems);
    test_norm = elastica::batch_norm(expanded_vector);
    VT correct_norm{std::sqrt(14.0)};
    //! [batch_norm_example]

    CHECK(test_norm == correct_norm);
  }
}  // namespace
