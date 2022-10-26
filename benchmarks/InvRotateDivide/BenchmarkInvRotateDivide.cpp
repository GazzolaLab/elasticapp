//
// Created by Tejaswin Parthasarathy on 1/20/22.
//
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING  // benchmarking support
//
#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "ModuleSettings/Vectorization.hpp"
#include "Utilities/DefineTypes.h"
#include "catch.hpp"
//
#include "InvRotateDivideSetup.hpp"

std::size_t n_values = 1UL;
elastica::real_t inclination = 0.;

TEST_CASE("BenchmarkInvRotateDivide") {
  using namespace elastica::cosserat_rod::detail;

  BENCHMARK_ADVANCED("Inv rotate blaze")(Catch::Benchmark::Chronometer meter) {
    InvRotateDivideTimingSetup<InvRotateDivideKind::blaze> bench(n_values,
                                                                 inclination);
    meter.measure([&] {
      bench.run();
      return bench.kappa_output;
    });
  };

  BENCHMARK_ADVANCED("Inv rotate scalar")
  (Catch::Benchmark::Chronometer meter) {
    InvRotateDivideTimingSetup<InvRotateDivideKind::scalar> bench(n_values,
                                                                  inclination);
    meter.measure([&] {
      bench.run();
      return bench.kappa_output;
    });
  };

#if ELASTICA_USE_VECTORIZATION
  BENCHMARK_ADVANCED("Inv rotate SIMD")
  (Catch::Benchmark::Chronometer meter) {
    InvRotateDivideTimingSetup<InvRotateDivideKind::simd> bench(n_values,
                                                                inclination);
    meter.measure([&] {
      bench.run();
      return bench.kappa_output;
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
             | Opt(n_values, "n")["--n_values"]("how many values?") |
             Opt(inclination, "i")["--inclination"]("inclination");
  session.cli(cli);

  // Let Catch (using Clara) parse the command line
  int returnCode = session.applyCommandLine(argc, argv);
  if (returnCode != 0)  // Indicates a command line error
    return returnCode;

  int result = session.run(argc, argv);

  // global clean-up...

  return result;
}
