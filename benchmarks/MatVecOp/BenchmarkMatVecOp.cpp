//
// Created by Tejaswin Parthasarathy on 10/26/22.
//
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING  // benchmarking support
//
#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "ModuleSettings/Vectorization.hpp"
#include "catch.hpp"
//
#include "MatVecOpSetup.hpp"

std::size_t n_values = 1UL;

TEST_CASE("BenchmarkMatVecOp") {
  using namespace elastica::cosserat_rod;

  BENCHMARK_ADVANCED("MatVec blaze")(Catch::Benchmark::Chronometer meter) {
    MatVecOpTimingSetup<MatVecKind::blaze> bench(n_values);
    meter.measure([&] {
      bench.run();
      return std::ref(bench.mat_input);
    });
  };

  BENCHMARK_ADVANCED("MatVec scalar")
  (Catch::Benchmark::Chronometer meter) {
    MatVecOpTimingSetup<MatVecKind::scalar> bench(n_values);
    meter.measure([&] {
      bench.run();
      return std::ref(bench.mat_input);
    });
  };

#if ELASTICA_USE_VECTORIZATION
  BENCHMARK_ADVANCED("MatVec SIMD")
  (Catch::Benchmark::Chronometer meter) {
    MatVecOpTimingSetup<MatVecKind::simd> bench(n_values);
    meter.measure([&] {
      bench.run();
      return std::ref(bench.mat_input);
    });
  };
#endif
}

int main(int argc, char* argv[]) {
  // global setup...
  disable_floating_point_exceptions();

  Catch::Session session;

  // Build a new parser on top of Catch2's
  using namespace Catch::clara;
  auto cli = session.cli()  // Get Catch2's command line parser
             | Opt(n_values, "n")["--n_values"]("how many values?");
  session.cli(cli);

  // Let Catch (using Clara) parse the command line
  int returnCode = session.applyCommandLine(argc, argv);
  if (returnCode != 0)  // Indicates a command line error
    return returnCode;

  int result = session.run(argc, argv);

  // global clean-up...

  return result;
}
