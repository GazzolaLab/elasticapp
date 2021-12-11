//
// Created by Tejaswin Parthasarathy on 12/11/21.
//

#include <cstdlib>

#include "SO3PrimitiveAddAssignSetup.hpp"

int main() {
  using namespace elastica::states::detail;

  SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::scalar> scalar_bench{
      256UL};
  SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::simd> simd_bench{256UL};

  const std::size_t n_samples(100000UL);
  //  for (std::size_t i = 0; i < n_samples; ++i)
  //    scalar_bench.run();

  for (std::size_t i = 0; i < n_samples; ++i)
    simd_bench.run();

  return EXIT_SUCCESS;
}
