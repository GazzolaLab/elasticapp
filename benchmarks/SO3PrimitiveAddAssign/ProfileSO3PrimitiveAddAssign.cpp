//
// Created by Tejaswin Parthasarathy on 12/11/21.
//

#include <cstdlib>
#include <iostream>

#include "SO3PrimitiveAddAssignSetup.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAssign.hpp"

int main() {
  using namespace elastica::states::detail;

  const std::size_t n_values{1 << 16};
  const std::size_t n_samples(1 << 14);
  std::cout << "(" << n_values << ", " << n_samples << ")" << std::endl;

  SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::scalar> scalar_bench(
      n_values);
  //  SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::simd>
  //  simd_bench{n_values};

  //  using H_mat = blaze::DynamicTensor<::elastica::real_t>;
  //  using DH_mat = blaze::DynamicMatrix<::elastica::real_t>;
  //  H_mat Q_output(3UL, 3UL, n_values);
  //  H_mat Q_input(3UL, 3UL, n_values);
  //  blaze::randomize(Q_input);
  //  DH_mat w_input(3UL, n_values);
  //  blaze::randomize(w_input);

  for (std::size_t i = 0; i < n_samples; ++i)
    scalar_bench.run();

  //  for (std::size_t i = 0; i < n_samples; ++i)
  //    simd_bench.run();

  //  for (std::size_t i = 0; i < n_samples; ++i)
  //    ::elastica::states::SO3_primitive_assign(Q_output, Q_input, w_input);

  return EXIT_SUCCESS;
}
