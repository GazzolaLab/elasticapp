//
// Created by Tejaswin Parthasarathy on 10/26/22.
//
#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "MatVecOpSetup.hpp"
#include "ModuleSettings/Vectorization.hpp"
//
#include <cstdlib>
#include <iostream>
#include <string>

std::string getCmdOption(int argc, const char* argv[],
                         const std::string& option) {
  std::string cmd;
  for (int i = 0; i < argc; ++i) {
    std::string arg = argv[i];
    if (0 == arg.find(option)) {
      std::size_t found = arg.find_first_of('=');
      cmd = arg.substr(found + 1);
      return cmd;
    }
  }
  return cmd;
}

int main(const int argc, const char** argv) {
  using namespace elastica::cosserat_rod;
  disable_floating_point_exceptions();

  std::string n_values_as_str = getCmdOption(argc, argv, "--n_values=");
  std::string n_samples_as_str = getCmdOption(argc, argv, "--n_samples=");

  const std::size_t n_values = std::stoul(n_values_as_str, nullptr, 0);
  const std::size_t n_samples = std::stoul(n_samples_as_str, nullptr, 0);
  std::cout << "(" << n_values << ", " << n_samples << ")" << std::endl;

  MatVecOpTimingSetup<MatVecKind::blaze> blaze_bench(n_values);
  MatVecOpTimingSetup<MatVecKind::scalar> scalar_bench(n_values);
#if ELASTICA_USE_VECTORIZATION
  MatVecOpTimingSetup<MatVecOpKind::simd> simd_bench(n_values);
#endif

  for (std::size_t i = 0; i < n_samples; ++i)
    scalar_bench.run();

  return EXIT_SUCCESS;
}
