//
// Created by Tejaswin Parthasarathy on 1/20/22.
//

#include <cstdlib>

#include "InvRotateDivideSetup.hpp"

int main() {
  using namespace elastica::cosserat_rod::detail;

  const std::size_t n_elems{256UL};
  InvRotateDivideTimingSetup<InvRotateDivideKind::blaze> blaze_bench{n_elems};
  InvRotateDivideTimingSetup<InvRotateDivideKind::scalar> scalar_bench{n_elems};
  InvRotateDivideTimingSetup<InvRotateDivideKind::simd> simd_bench{256UL};

  const std::size_t n_samples(100000UL);

  for (std::size_t i = 0; i < n_samples; ++i)
    blaze_bench.run();

  return EXIT_SUCCESS;
}
