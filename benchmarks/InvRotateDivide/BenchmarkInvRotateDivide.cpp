//
// Created by Tejaswin Parthasarathy on 1/20/22.
//
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING  // benchmarking support
//
#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "catch.hpp"
//
#include "InvRotateDivideSetup.hpp"

TEST_CASE("BenchmarkInvRotateDivide") {
  using namespace elastica::cosserat_rod::detail;
  const std::size_t n_values = 1024UL;

  BENCHMARK_ADVANCED("Inv rotate blaze")(Catch::Benchmark::Chronometer meter) {
    InvRotateDivideTimingSetup<InvRotateDivideKind::blaze> bench{n_values};
    meter.measure([&] {
      bench.run();
      return bench.kappa_output;
    });
  };

  BENCHMARK_ADVANCED("Inv rotate scalar")
  (Catch::Benchmark::Chronometer meter) {
    InvRotateDivideTimingSetup<InvRotateDivideKind::scalar> bench{n_values};
    meter.measure([&] {
      bench.run();
      return bench.kappa_output;
    });
  };

  //  BENCHMARK_ADVANCED("Inv rotate SIMD")
  //  (Catch::Benchmark::Chronometer meter) {
  //    InvRotateDivideTimingSetup<InvRotateDivideKind::simd>
  //    bench{n_values}; meter.measure([&] {
  //      bench.run();
  //      return bench.w_input;
  //    });
  //  };
}

int main(int argc, char* argv[]) {
  // global setup...
  enable_floating_point_exceptions();

  int result = Catch::Session().run(argc, argv);

  // global clean-up...

  return result;
}
