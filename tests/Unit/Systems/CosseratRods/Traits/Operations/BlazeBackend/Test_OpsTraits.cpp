//
// Created by Seung Hyun Kim on 10/3/21.
//

#include "Framework/TestingFramework.hpp"

#include "elastica/Systems/CosseratRods/Traits/Operations/BlazeBackend/OpsTraits.hpp"
#include "elastica/Systems/CosseratRods/Traits/Operations/BlazeBackend/OptimizationLevel.hpp"

#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

namespace { // TODO: Operation trait unit test separate from blaze utility-kernels

  using ValueType = double;
  using VT = blaze::DynamicVector<ValueType, blaze::rowVector>;
  using MT = blaze::DynamicMatrix<ValueType, blaze::rowMajor>;
  using TT = blaze::DynamicTensor<ValueType>;
  using Operations=::elastica::cosserat_rod::Operations<::elastica::cosserat_rod::OptimizationLevel::basic>;
  constexpr std::size_t dimension(3UL);

  ELASTICA_TEST_CASE("Blaze test: Randomised batch matrix-vector product test",
                     "[Unit][Utilities]") {
    for (std::size_t n_elems : {1, 32, 128}) {
      TT matrix_batch(dimension, dimension, n_elems);
      MT vector_batch(dimension, n_elems),
          test_matvec_batch(dimension, n_elems),
          correct_matvec_batch(dimension, n_elems);
      blaze::randomize(matrix_batch);
      blaze::randomize(vector_batch);
      Operations::batch_matvec(test_matvec_batch, matrix_batch, vector_batch);

      /* element-wise matvec product */
      for (std::size_t i(0UL); i < n_elems; ++i)
        blaze::column(correct_matvec_batch, i) =
            blaze::columnslice(matrix_batch, i) *
            blaze::column(vector_batch, i);

      CHECK(test_matvec_batch == correct_matvec_batch);
    }
  }
} // namespace