//
// Created by Tejaswin Parthasarathy on 12/11/21.
//

#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING  // benchmarking support
//
#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "catch.hpp"
//
#include "SO3PrimitiveAddAssignSetup.hpp"

TEST_CASE("BenchmarkSO3PrimitiveAddAssign") {
  using namespace elastica::states::detail;

  BENCHMARK_ADVANCED("Add assign scalar")
  (Catch::Benchmark::Chronometer meter) {
    SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::scalar> bench{256UL};
    meter.measure([&] {
      bench.run();
      return bench.w_input;
    });
  };

  BENCHMARK_ADVANCED("Add assign SIMD")
  (Catch::Benchmark::Chronometer meter) {
    SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::simd> bench{256UL};
    meter.measure([&] {
      bench.run();
      return bench.w_input;
    });
  };
}

int main(int argc, char* argv[]) {
  // global setup...
  enable_floating_point_exceptions();

  int result = Catch::Session().run(argc, argv);

  // global clean-up...

  return result;
}
