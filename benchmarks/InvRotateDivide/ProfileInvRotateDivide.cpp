//
// Created by Tejaswin Parthasarathy on 1/20/22.
//
#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "InvRotateDivideSetup.hpp"
#include "ModuleSettings/Vectorization.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/IgnoreUnused.hpp"
//
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

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
  using namespace elastica::cosserat_rod::detail;
  disable_floating_point_exceptions();

  std::string n_values_as_str = getCmdOption(argc, argv, "--n_values=");
  std::string n_samples_as_str = getCmdOption(argc, argv, "--n_samples=");
  std::string n_objects_as_str = getCmdOption(argc, argv, "--n_objects=");
  std::string angle_as_str = getCmdOption(argc, argv, "--inclination=");

  const std::size_t n_values = std::stoul(n_values_as_str, nullptr, 0);
  const std::size_t n_samples = std::stoul(n_samples_as_str, nullptr, 0);
  const std::size_t n_objects = std::stoul(n_objects_as_str, nullptr, 0);
  const ::elastica::real_t increment = std::stod(angle_as_str, nullptr);
  std::cout << "(" << n_values << ", " << n_samples << ")" << std::endl;

  std::vector<InvRotateDivideTimingSetup<InvRotateDivideKind::blaze>>
      blaze_benches;
  for (auto i = 0UL; i < n_objects; ++i)
    blaze_benches.emplace_back(n_values, increment);

  auto run_blaze_profile = [&]() {
    for (auto& blaze_bench : blaze_benches)
      blaze_bench.run();
  };

  std::vector<InvRotateDivideTimingSetup<InvRotateDivideKind::scalar>>
      scalar_benches;
  for (auto i = 0UL; i < n_objects; ++i)
    scalar_benches.emplace_back(n_values, increment);

  auto run_scalar_profile = [&]() {
    for (auto& scalar_bench : scalar_benches)
      scalar_bench.run();
  };

#if ELASTICA_USE_VECTORIZATION
  std::vector<InvRotateDivideTimingSetup<InvRotateDivideKind::simd>>
      simd_benches;
  for (auto i = 0UL; i < n_objects; ++i)
    simd_benches.emplace_back(n_values, increment);

  auto run_simd_profile = [&]() {
    for (auto& simd_bench : simd_benches)
      simd_bench.run();
  };

  elastica::ignore_unused(run_simd_profile);
#endif

  elastica::ignore_unused(run_blaze_profile, run_scalar_profile);

  for (std::size_t i = 0; i < n_samples; ++i)
    run_scalar_profile();

#if ELASTICA_USE_VECTORIZATION && 0
  for (std::size_t i = 0; i < n_samples; ++i)
    run_simd_profile();
#endif

#if 0
  for (std::size_t i = 0; i < n_samples; ++i)
    run_blaze_profile();
#endif

  return EXIT_SUCCESS;
}
