//
// Created by Tejaswin Parthasarathy on 1/20/22.
//

#include <cstdlib>
#include <iostream>

#include "InvRotateDivideSetup.hpp"
#include "ModuleSettings/Vectorization.hpp"

int main() {
  using namespace elastica::cosserat_rod::detail;

  const std::size_t n_values{1 << 16};
  const std::size_t n_samples(1 << 14);
  std::cout << "(" << n_values << ", " << n_samples << ")" << std::endl;

  InvRotateDivideTimingSetup<InvRotateDivideKind::blaze> blaze_bench{n_values};
  InvRotateDivideTimingSetup<InvRotateDivideKind::scalar> scalar_bench{
      n_values};
#if ELASTICA_USE_VECTORIZATION
  InvRotateDivideTimingSetup<InvRotateDivideKind::simd> simd_bench{n_values};
#endif

  for (std::size_t i = 0; i < n_samples; ++i)
    scalar_bench.run();

  return EXIT_SUCCESS;
}
