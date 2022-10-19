#include <elastica/Simulator/Configuration/ModuleConfiguration/BlockingPolicyStreams.hpp>
#include <elastica/Simulator/IO/YAML.hpp>
#include <elastica/Simulator/Utilities/Serialize.hpp>
#include <elastica/Utilities/AppSupport/OptionsParser/OptionsParser.hpp>
#include <elastica/Utilities/StdHelpers.hpp>
#include <memory>

#include "ParallelButterflyCaseStudy.hpp"

// Parallelism headers should already be included
#include <tbb/global_control.h>

struct ParallelButterflyCaseStudyWithBlocking
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

  ~ParallelButterflyCaseStudyWithBlocking() {
    // release the pointer : it will get cleaned up with the downstream
    // destructor
    simulator->blocking_policy().release();
  }

  static std::string name() { return "ParallelButterflyCaseStudy"; }
};

int main(int argc, char* argv[]) {
  using CaseStudy = ParallelButterflyCaseStudyWithBlocking;
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
  case_study.run_for_n_iterations(10000UL);
  record_profiling_data(case_study);

  std::cout << "Finished execution and wrote profiling information"
            << std::endl;

  return EXIT_SUCCESS;
}
