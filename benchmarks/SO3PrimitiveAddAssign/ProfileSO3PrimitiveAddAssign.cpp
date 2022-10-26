//
// Created by Tejaswin Parthasarathy on 12/11/21.
//

#include <cstdlib>
#include <iostream>

#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "ModuleSettings/Vectorization.hpp"
#include "SO3PrimitiveAddAssignSetup.hpp"
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAssign.hpp"

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
  using namespace elastica::states::detail;

  disable_floating_point_exceptions();

  std::string n_values_as_str = getCmdOption(argc, argv, "--n_values=");
  std::string n_samples_as_str = getCmdOption(argc, argv, "--n_samples=");
  std::string angle_as_str = getCmdOption(argc, argv, "--inclination=");

  const std::size_t n_values = std::stoul(n_values_as_str, nullptr, 0);
  const std::size_t n_samples = std::stoul(n_samples_as_str, nullptr, 0);
  const ::elastica::real_t increment = std::stod(angle_as_str, nullptr);

  std::cout << "(" << n_values << ", " << n_samples << ")" << std::endl;

  SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::scalar> scalar_bench(
      n_values, increment);
#if ELASTICA_USE_VECTORIZATION
  SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::simd> simd_bench(
      n_values, increment);
#endif

  for (std::size_t i = 0; i < n_samples; ++i)
    scalar_bench.run();

#if ELASTICA_USE_VECTORIZATION && 0
  for (std::size_t i = 0; i < n_samples; ++i)
    simd_bench.run();
#endif

#if 0
  for (std::size_t i = 0; i < n_samples; ++i)
    ::elastica::states::SO3_primitive_assign(
        scalar_bench.Q_input, scalar_bench.Q_input, scalar_bench.w_input);
#endif

  return EXIT_SUCCESS;
}
