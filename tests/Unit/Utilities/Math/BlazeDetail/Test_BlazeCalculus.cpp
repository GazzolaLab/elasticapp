//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do

#include "Framework/TestingFramework.hpp"
// #include "catch2/catch.hpp"

#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

#include "Utilities/Math/BlazeDetail/BlazeCalculus.hpp"

namespace {

  using ValueType = double;
  using VT = blaze::DynamicVector<ValueType, blaze::rowVector>;
  using MT = blaze::DynamicMatrix<ValueType, blaze::rowMajor>;
  constexpr std::size_t dimension(3UL);

  ELASTICA_TEST_CASE("Trapezoidal quadrature test for f(x) = 1",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {1, 32, 128}) {
      //! [quadrature_kernel_example]
      const std::size_t n_nodes = n_elems + 1UL;
      MT input_vector_batch(dimension, n_elems),
          test_vector_batch(dimension, n_nodes),
          correct_vector_batch(dimension, n_nodes);
      input_vector_batch = ValueType(1.0);
      elastica::quadrature_kernel(test_vector_batch, input_vector_batch);
      correct_vector_batch = ValueType(1.0);
      blaze::column(correct_vector_batch, 0UL) = ValueType(0.5);
      blaze::column(correct_vector_batch, n_nodes - 1UL) = ValueType(0.5);
      //! [quadrature_kernel_example]

      CHECK(test_vector_batch == correct_vector_batch);
    }
  }

  ELASTICA_TEST_CASE("Two point difference test for f(x) = 1",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {1, 32, 128}) {
      //! [difference_kernel_example]
      const std::size_t n_nodes = n_elems + 1UL;
      MT input_vector_batch(dimension, n_elems),
          test_vector_batch(dimension, n_nodes),
          correct_vector_batch(dimension, n_nodes);
      input_vector_batch = ValueType(1.0);
      elastica::two_point_difference_kernel(test_vector_batch, input_vector_batch);
      correct_vector_batch = ValueType(0.0);
      blaze::column(correct_vector_batch, 0UL) = ValueType(1.0);
      blaze::column(correct_vector_batch, n_nodes - 1UL) = ValueType(-1.0);
      //! [difference_kernel_example]

      CHECK(test_vector_batch == correct_vector_batch);
    }
  }

  ELASTICA_TEST_CASE("Trapezoidal quadrature test for f(x) = sin(x)",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {64, 128, 256}) {
      const std::size_t n_voronoi = n_elems - 1;
      MT input_vector_batch(dimension, n_elems),
          test_vector_batch(dimension, n_voronoi),
          correct_vector_batch(dimension, n_voronoi);
      const ValueType a = 0.0;
      const ValueType b = M_PI;
      const ValueType dh = (b - a) / (n_elems - 1);
      VT temp_vector =
          blaze::sin(blaze::linspace<blaze::rowVector>(n_elems, a, b));
      input_vector_batch =
          dh * blaze::submatrix(blaze::expand(temp_vector, dimension), 0UL, 1UL,
                                dimension, n_voronoi - 1UL);
      elastica::quadrature_kernel(test_vector_batch, input_vector_batch);

      /* Sampling for the midpoint rule needs to be done */
      /* one a grid that lies in between the actual function for */
      /* second-order accuracy! */
      const ValueType interior_a = a + 0.5 * dh;
      const ValueType interior_b = b - 0.5 * dh;
      temp_vector = blaze::sin(
          blaze::linspace<blaze::rowVector>(n_voronoi, interior_a, interior_b));
      correct_vector_batch = dh * blaze::expand(temp_vector, dimension);

      /* Tolerances set according to values in PyElastica */
      /* TODO 1e-3 and 1e-4 need to be refactored */
      for (std::size_t i(0UL); i < dimension; ++i)
        CHECK(blaze::sum(blaze::row(test_vector_batch, i)) ==
              Approx(ValueType(2.0)).margin(ValueType(1e-3)));
      CHECK((test_vector_batch - correct_vector_batch) ==
            Approx(ValueType(0.0)).margin(ValueType(1e-4)));
    }
  }

  ELASTICA_TEST_CASE("Two point difference test for f(x) = sin(x)",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {128, 256, 512}) {
      const std::size_t n_voronoi = n_elems - 1;
      MT input_vector_batch(dimension, n_elems),
          test_vector_batch(dimension, n_voronoi),
          correct_vector_batch(dimension, n_voronoi);
      const ValueType a = 0.0;
      const ValueType b = M_PI;
      const ValueType dh = (b - a) / (n_elems - 1);
      VT temp_vector =
          blaze::sin(blaze::linspace<blaze::rowVector>(n_elems, a, b));
      input_vector_batch =
          blaze::submatrix(blaze::expand(temp_vector, dimension), 0UL, 1UL,
                           dimension, n_voronoi - 1UL) /
          dh;
      elastica::two_point_difference_kernel(test_vector_batch, input_vector_batch);

      /* Sampling for the analytical derivative needs to be done */
      /* one a grid that lies in between the actual function for */
      /* second-order accuracy! */
      const ValueType interior_a = a + 0.5 * dh;
      const ValueType interior_b = b - 0.5 * dh;
      temp_vector = blaze::cos(
          blaze::linspace<blaze::rowVector>(n_voronoi, interior_a, interior_b));
      correct_vector_batch = blaze::expand(temp_vector, dimension);

      /* Tolerance set according to values in PyElastica */
      /* TODO 1e-4 needs to be refactored */
      CHECK((test_vector_batch - correct_vector_batch) ==
            Approx(ValueType(0.0)).margin(ValueType(1e-4)));
    }
  }
} // namespace