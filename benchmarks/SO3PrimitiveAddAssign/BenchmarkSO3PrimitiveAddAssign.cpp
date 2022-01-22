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
#include "Systems/States/Expressions/backends/blaze/SO3PrimitiveAssign.hpp"

std::size_t n_values = 1UL;

TEST_CASE("BenchmarkSO3PrimitiveAddAssign") {
  using namespace elastica::states::detail;

  BENCHMARK_ADVANCED("SO3 Add assign via SO3 Assign ")
  (Catch::Benchmark::Chronometer meter) {
    using H_mat = blaze::DynamicTensor<::elastica::real_t>;
    using DH_mat = blaze::DynamicMatrix<::elastica::real_t>;
    H_mat Q_output(3UL, 3UL, n_values);
    H_mat Q_input(3UL, 3UL, n_values);
    blaze::randomize(Q_input);
    DH_mat w_input(3UL, n_values);
    blaze::randomize(w_input);

    meter.measure([&] {
      ::elastica::states::SO3_primitive_assign(Q_output, Q_input, w_input);
      return w_input;
    });
  };

  BENCHMARK_ADVANCED("Add assign scalar")
  (Catch::Benchmark::Chronometer meter) {
    SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::scalar> bench{
        n_values};
    meter.measure([&] {
      bench.run();
      return bench.w_input;
    });
  };

  BENCHMARK_ADVANCED("Add assign SIMD")
  (Catch::Benchmark::Chronometer meter) {
    SO3PrimitiveAddAssignBenchmarkSetup<SO3AddAssignKind::simd> bench{n_values};
    meter.measure([&] {
      bench.run();
      return bench.w_input;
    });
  };
}

int main(int argc, char* argv[]) {
  // global setup...
  disable_floating_point_exceptions();  // microbenchmark : so measure the best
                                        // version

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
