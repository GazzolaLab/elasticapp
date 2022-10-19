#include <memory>

#include "elastica/Simulator/Configuration/ModuleConfiguration/BlockingPolicyStreams.hpp"
#include "elastica/Simulator/IO/YAML.hpp"
#include "elastica/Simulator/Utilities/Serialize.hpp"
#include "elastica/Utilities/AppSupport/OptionsParser/OptionsParser.hpp"
#include "elastica/Utilities/StdHelpers.hpp"
#include "examples/parallel_butterfly/ParallelButterflyCaseStudy.hpp"

// Parallelism headers should already be included
#include <tbb/global_control.h>

struct ParallelButterflyNoSyncCaseStudyWithBlocking
    : public ParallelButterflyCaseStudy {
  void setup(InputTypes<ParallelButterflyCaseStudy> const& params) {
    // Change the simulator blocking here
    // hack to workaround non-copyable unique pointers. Params are always
    // stored downstream so they dont go out of scope. They will get deleted
    // eventually, so dont double delete in constructor
    simulator->blocking_policy().reset(
        tuples::get<BlockingPolicy>(params).get());
    ParallelButterflyCaseStudy::setup(params);
  }

  ~ParallelButterflyNoSyncCaseStudyWithBlocking() {
    // release the pointer : it will get cleaned up with the downstream
    // destructor
    simulator->blocking_policy().release();
  }

  static std::string name() { return "ParallelButterflyNoSyncCaseStudy"; }
};

int main(int argc, char* argv[]) {
  using CaseStudy = ParallelButterflyNoSyncCaseStudyWithBlocking;
  char const* case_study_name("parallel_butterfly");

  auto case_study =
      ::standalone_bindings::parse_case_study_from_options<CaseStudy>(
          case_study_name, argc, argv);
  std::cout << "Setup case-study" << std::endl;
  std::cout << case_study.parameters() << std::endl;
  // This delays execution of setup till users can confirm that "check" input
  // works as expected.
  case_study.setup();

  ::elastica::io::YAMLMetadata metadata{};

  auto record_profiling_data = [&](auto& study) {
    // This is a hack around YamlCPP deficiences
    ::elastica::io::YAMLOutputArchive writer{
        metadata.write_at({study.get_current_time(), study.get_dt(),
                           study.get_current_iteration()})};
    auto& container = study.simulator->get_system_collection_container();
    auto const n_blocks = std::get<0UL>(container).size();
    writer(::elastica::io::make_nvp("n_blocks", n_blocks));
    writer(::elastica::io::make_nvp("profiling", study.simulator->timers()));
  };

  // const int n_threads_used_by_application = 4;
  // controls the application level parallelism
  tbb::global_control application_limit(
      tbb::global_control::max_allowed_parallelism,
      tuples::get<typename CaseStudy::n_threads>(case_study.parameters().data));

  record_profiling_data(case_study);
  // Execution profile : control number of threads here
  auto const dt = case_study.get_dt();
  auto const time_v = case_study.get_current_time();

  // 2 for position verlet
  const auto n_iterations = 100UL;

  case_study.simulator->for_each_block([=](auto& block) {
    using KinematicStep =
        ::elastica::timestepper::symplectic::PositionVerlet::KinematicSubStep;
    using DynamicStep =
        ::elastica::timestepper::symplectic::PositionVerlet::DynamicSubStep;
    KinematicStep kinematic_step{};
    DynamicStep dynamic_step{};

    for (auto tid = 0; tid < n_iterations; ++tid) {
      // pos update
      kinematic_step(block, time_v, dt);
      // dynamics update
      dynamic_step(block, time_v, dt);
      // pos update
      kinematic_step(block, time_v, dt);
    }
  });

  case_study.get_current_iteration().value() = n_iterations;
  case_study.get_current_time().value() =
      static_cast<::elastica::real_t>(case_study.get_dt()) * n_iterations;

  record_profiling_data(case_study);

  std::cout << "Finished execution and wrote profiling information"
            << std::endl;

  return EXIT_SUCCESS;
}
