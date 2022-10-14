//
// Created by Tejaswin Parthasarathy on 10/11/22.
//
#include <cmath>

#include "Framework/TestingFramework.hpp"
#include "ModuleSettings/Vectorization.hpp"
#include "Utilities/TMPL.hpp"
#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"
//
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/LinearAlgebra/Kernels.hpp"

namespace {

  using ValueType = double;
  using VT = blaze::DynamicVector<ValueType>;  //, blaze::rowVector>;
  using MT = blaze::DynamicMatrix<ValueType>;
  using TT = blaze::DynamicTensor<ValueType>;
  constexpr std::size_t dimension(3UL);

  using elastica::cosserat_rod::MatVecKind;
  using elastica::cosserat_rod::MatVecOp;

  template <MatVecKind K>
  void run_matvec_test_randomized(MatVecOp<K> op) {
    for (std::size_t n_elems : {1UL, 4UL, 32UL, 33UL, 34UL, 35UL, 36UL}) {
      TT matrix_batch(dimension, dimension, n_elems);
      MT vector_batch(dimension, n_elems),
          test_matvec_batch(dimension, n_elems),
          correct_matvec_batch(dimension, n_elems);
      blaze::randomize(matrix_batch);
      blaze::randomize(vector_batch);
      op.apply(test_matvec_batch, matrix_batch, vector_batch);

      /* element-wise matvec product */
      for (std::size_t i(0UL); i < n_elems; ++i)
        blaze::column(correct_matvec_batch, i) =
            blaze::columnslice(matrix_batch, i) *
            blaze::column(vector_batch, i);

      CHECK(test_matvec_batch == correct_matvec_batch);
    }
  }

  template <MatVecKind K>
  void run_matvec_test_analytical(MatVecOp<K> op) {
    static Approx approx = Approx::custom().epsilon(1e-5);
    TT matrix_batch(3UL, 3UL, 9UL);
    MT vector_batch(3UL, 9UL), test_matvec_batch(3UL, 9UL),
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

    op.apply(test_matvec_batch, matrix_batch, vector_batch);

    CHECK_MATRIX_CUSTOM_APPROX(test_matvec_batch, correct_matvec_batch, approx);
  }

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operations.MatVec",
                     "[Unit][Systems]") {
    using TestKernelKinds =
        tmpl::integral_list<MatVecKind, MatVecKind::blaze, MatVecKind::scalar
#if ELASTICA_USE_VECTORIZATION
                            ,
                            MatVecKind::simd
#endif
                            >;

    auto for_each_backend = [](auto func) {
      tmpl::for_each<TestKernelKinds>([=](auto k) {
        using Kernel = tmpl::type_from<decltype(k)>;
        SECTION(to_string(Kernel::value)) { func(MatVecOp<Kernel::value>{}); }
      });
    };

    SECTION("Randomized") {
      for_each_backend(
          [](auto op) { run_matvec_test_randomized(std::move(op)); });
    }
    SECTION("Analytical") {
      for_each_backend(
          [](auto op) { run_matvec_test_analytical(std::move(op)); });
    }
  }

  using elastica::cosserat_rod::VecScalarDivKind;
  using elastica::cosserat_rod::VecScalarDivOp;

  template <VecScalarDivKind K>
  void run_vecscalardiv_test_randomized(VecScalarDivOp<K> op) {
    // Test out non-multiples as well
    for (std::size_t n_elems :
         {1UL, 4UL, 32UL, 33UL, 34UL, 35UL, 36UL, 37UL, 38UL, 39UL, 40UL}) {
      VT scalar_batch(n_elems);
      MT vector_batch(dimension, n_elems),
          test_vecscaldiv_batch(dimension, n_elems),
          correct_vecscaldiv_batch(dimension, n_elems);
      blaze::randomize(vector_batch);
      blaze::randomize(scalar_batch);
      op.apply(test_vecscaldiv_batch, vector_batch, scalar_batch);

      /* element-wise vecscaldiv product */
      for (std::size_t i(0UL); i < n_elems; ++i)
        blaze::column(correct_vecscaldiv_batch, i) =
            blaze::column(vector_batch, i) / scalar_batch[i];

      CHECK(test_vecscaldiv_batch == correct_vecscaldiv_batch);
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operations.VecScalarDiv",
                     "[Unit][Systems]") {
    using TestKernelKinds =
        tmpl::integral_list<VecScalarDivKind, VecScalarDivKind::blaze,
                            VecScalarDivKind::scalar
#if ELASTICA_USE_VECTORIZATION
                            ,
                            VecScalarDivKind::simd
#endif
                            >;

    auto for_each_backend = [](auto func) {
      tmpl::for_each<TestKernelKinds>([=](auto k) {
        using Kernel = tmpl::type_from<decltype(k)>;
        SECTION(to_string(Kernel::value)) {
          func(VecScalarDivOp<Kernel::value>{});
        }
      });
    };

    SECTION("Randomized") {
      for_each_backend(
          [](auto op) { run_vecscalardiv_test_randomized(std::move(op)); });
    }
  }

  using elastica::cosserat_rod::CrossProductKind;
  using elastica::cosserat_rod::CrossProductOp;

  template <CrossProductKind K>
  void run_crossproduct_test_randomized(CrossProductOp<K> op) {
    for (std::size_t n_elems :
         {1UL, 4UL, 32UL, 33UL, 34UL, 35UL, 36UL, 37UL, 38UL, 39UL}) {
      MT first_vector_batch(dimension, n_elems),
          second_vector_batch(dimension, n_elems),
          test_cross_product_batch(dimension, n_elems),
          correct_cross_product_batch(dimension, n_elems);
      blaze::randomize(first_vector_batch);
      blaze::randomize(second_vector_batch);
      op.apply(test_cross_product_batch, first_vector_batch,
               second_vector_batch);

      /* element-wise cross product_batch */
      for (std::size_t i(0UL); i < n_elems; ++i)
        blaze::column(correct_cross_product_batch, i) =
            blaze::cross(blaze::column(first_vector_batch, i),
                         blaze::column(second_vector_batch, i));

      CHECK(test_cross_product_batch == correct_cross_product_batch);
    }
  }

  template <CrossProductKind K>
  void run_crossproduct_test_analytical(CrossProductOp<K> op) {
    for (std::size_t n_elems :
         {1UL, 4UL, 32UL, 33UL, 34UL, 35UL, 36UL, 37UL, 38UL, 39UL}) {
      const VT first_vector{1.0, 2.0, 3.0};
      const VT second_vector{4.0, 5.0, 6.0};
      const VT correct_product{-3.0, 6.0, -3.0};

      MT expanded_first_vector(3UL, n_elems);
      MT expanded_second_vector(3UL, n_elems);
      MT correct_cross_product(3UL, n_elems);

      for (std::size_t i(0UL); i < n_elems; ++i) {
        blaze::column(expanded_first_vector, i) = first_vector;
        blaze::column(expanded_second_vector, i) = second_vector;
        blaze::column(correct_cross_product, i) = correct_product;
      }

      MT test_cross_product(dimension, n_elems);

      op.apply(test_cross_product, expanded_first_vector,
               expanded_second_vector);

      CHECK(test_cross_product == correct_cross_product);
    }
  }

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.Traits.Operations.CrossProduct",
                     "[Unit][Systems]") {
    using TestKernelKinds =
        tmpl::integral_list<CrossProductKind, CrossProductKind::blaze,
                            CrossProductKind::scalar
#if ELASTICA_USE_VECTORIZATION
                            ,
                            CrossProductKind::simd
#endif
                            >;

    auto for_each_backend = [](auto func) {
      tmpl::for_each<TestKernelKinds>([=](auto k) {
        using Kernel = tmpl::type_from<decltype(k)>;
        SECTION(to_string(Kernel::value)) {
          func(CrossProductOp<Kernel::value>{});
        }
      });
    };

    SECTION("Randomized") {
      for_each_backend(
          [](auto op) { run_crossproduct_test_randomized(std::move(op)); });
    }
    SECTION("Analytical") {
      for_each_backend(
          [](auto op) { run_crossproduct_test_analytical(std::move(op)); });
    }
  }

}  // namespace