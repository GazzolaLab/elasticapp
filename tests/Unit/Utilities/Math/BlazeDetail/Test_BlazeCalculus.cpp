#include "Framework/TestingFramework.hpp"
#include "Utilities/Math/BlazeDetail/BlazeCalculus.hpp"
#include "Utilities/TMPL.hpp"
#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

namespace {

  using ValueType = double;
  using VT = blaze::DynamicVector<ValueType, blaze::rowVector>;
  using MT = blaze::DynamicMatrix<ValueType, blaze::rowMajor>;
  constexpr std::size_t dimension(3UL);

  enum class QuadratureKind { blaze };
  template <QuadratureKind OpKind>
  struct QuadratureOp;

  template <>
  struct QuadratureOp<QuadratureKind::blaze> {
    template <typename... Args>  // blaze Matrix expression type
    static inline auto apply(Args&&... args) noexcept -> void {
      elastica::quadrature_kernel(std::forward<Args>(args)...);
    };
  };

  std::string to_string(QuadratureKind k) { return "QuadratureKindBlaze"; }

  template <QuadratureKind K>
  void run_quadrature_test_1(QuadratureOp<K> op) {
    for (std::size_t n_elems :
         {1UL, 4UL, 32UL, 33UL, 34UL, 35UL, 36UL, 37UL, 38UL, 39UL}) {
      const std::size_t n_nodes = n_elems + 1UL;
      MT input_vector_batch(dimension, n_elems),
          test_vector_batch(dimension, n_nodes),
          correct_vector_batch(dimension, n_nodes);
      input_vector_batch = ValueType(2.0);
      correct_vector_batch = ValueType(2.0);
      test_vector_batch = ValueType(0.0);
      blaze::column(correct_vector_batch, 0UL) = ValueType(1.0);
      blaze::column(correct_vector_batch, n_nodes - 1UL) = ValueType(1.0);
      //! [quadrature_kernel_example]
      op.apply(test_vector_batch, input_vector_batch);
      //! [quadrature_kernel_example]
      CHECK(test_vector_batch == correct_vector_batch);
    }
  };

  template <QuadratureKind K>
  void run_quadrature_test_2(QuadratureOp<K> op) {
    for (std::size_t n_elems :
         {128UL, 129UL, 130UL, 131UL, 132UL, 133UL, 134UL, 135UL}) {
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
      op.apply(test_vector_batch, input_vector_batch);

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

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeCalculus.Quadrature",
                     "[Unit][Utilities]") {
    using TestKernelKinds =
        tmpl::integral_list<QuadratureKind, QuadratureKind::blaze>;

    SECTION("f(x) = 1") {
      tmpl::for_each<TestKernelKinds>([](auto k) {
        using Kernel = tmpl::type_from<decltype(k)>;
        SECTION(to_string(Kernel::value)) {
          run_quadrature_test_1(QuadratureOp<Kernel::value>{});
        }
      });
    }
    SECTION("f(x) = sin(x)") {
      tmpl::for_each<TestKernelKinds>([](auto k) {
        using Kernel = tmpl::type_from<decltype(k)>;
        SECTION(to_string(Kernel::value)) {
          run_quadrature_test_2(QuadratureOp<Kernel::value>{});
        }
      });
    }
  }

  enum class TwoPointDifferenceKind { blaze };
  template <TwoPointDifferenceKind OpKind>
  struct TwoPointDifferenceOp;

  template <>
  struct TwoPointDifferenceOp<TwoPointDifferenceKind::blaze> {
    template <typename... Args>  // blaze Matrix expression type
    static inline auto apply(Args&&... args) noexcept -> void {
      elastica::two_point_difference_kernel(std::forward<Args>(args)...);
    };
  };

  std::string to_string(TwoPointDifferenceKind k) {
    return "TwoPointDifferenceKindBlaze";
  }

  template <TwoPointDifferenceKind K>
  void run_difference_test_1(TwoPointDifferenceOp<K> op) {
    for (std::size_t n_elems :
         {1UL, 4UL, 32UL, 33UL, 34UL, 35UL, 36UL, 37UL, 38UL, 39UL}) {
      //! [difference_kernel_example]
      const std::size_t n_nodes = n_elems + 1UL;
      MT input_vector_batch(dimension, n_elems),
          test_vector_batch(dimension, n_nodes),
          correct_vector_batch(dimension, n_nodes);
      input_vector_batch = ValueType(1.0);
      op.apply(test_vector_batch, input_vector_batch);
      correct_vector_batch = ValueType(0.0);
      blaze::column(correct_vector_batch, 0UL) = ValueType(1.0);
      blaze::column(correct_vector_batch, n_nodes - 1UL) = ValueType(-1.0);
      //! [difference_kernel_example]

      CHECK(test_vector_batch == correct_vector_batch);
    }
  }

  template <TwoPointDifferenceKind K>
  void run_difference_test_2(TwoPointDifferenceOp<K> op) {
    for (std::size_t n_elems :
         {128UL, 129UL, 130UL, 131UL, 132UL, 133UL, 134UL, 135UL}) {
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
      op.apply(test_vector_batch, input_vector_batch);

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

  ELASTICA_TEST_CASE("Unit.Utilities.Math.BlazeDetail.BlazeCalculus.Difference",
                     "[Unit][Utilities]") {
    using TestKernelKinds = tmpl::integral_list<TwoPointDifferenceKind,
                                                TwoPointDifferenceKind::blaze>;

    SECTION("f(x) = 1") {
      tmpl::for_each<TestKernelKinds>([=](auto k) {
        using Kernel = tmpl::type_from<decltype(k)>;
        SECTION(to_string(Kernel::value)) {
          run_difference_test_1(TwoPointDifferenceOp<Kernel::value>{});
        }
      });
    }
    SECTION("f(x) = sin(x)") {
      tmpl::for_each<TestKernelKinds>([=](auto k) {
        using Kernel = tmpl::type_from<decltype(k)>;
        SECTION(to_string(Kernel::value)) {
          run_difference_test_2(TwoPointDifferenceOp<Kernel::value>{});
        }
      });
    }
  }

}  // namespace