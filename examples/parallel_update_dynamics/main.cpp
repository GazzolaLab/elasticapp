#include <chrono>
#include <elastica/Simulator/Configuration/ModuleConfiguration/BlockingPolicyStreams.hpp>
#include <elastica/Simulator/IO/YAML.hpp>
#include <elastica/Simulator/Utilities/Serialize.hpp>
#include <elastica/Utilities/AppSupport/OptionsParser/OptionsParser.hpp>
#include <elastica/Utilities/StdHelpers.hpp>
#include <memory>

#include "ParallelUpdateDynamicsCaseStudy.hpp"

// Parallelism headers should already be included
#include <tbb/global_control.h>

struct ParallelUpdateDynamicsCaseStudyWithBlocking
    : public ParallelUpdateDynamicsCaseStudy {
  void setup(InputTypes<ParallelUpdateDynamicsCaseStudy> const& params) {
    // Change the simulator blocking here
    // hack to workaround non-copyable unique pointers. Params are always
    // stored downstream so they dont go out of scope. They will get deleted
    // eventually, so dont double delete in constructor
    simulator->blocking_policy().reset(
        tuples::get<BlockingPolicy>(params).get());
    ParallelUpdateDynamicsCaseStudy::setup(params);
  }

  ~ParallelUpdateDynamicsCaseStudyWithBlocking() {
    // release the pointer : it will get cleaned up with the downstream
    // destructor
    simulator->blocking_policy().release();
  }

  static std::string name() { return "ParallelUpdateDynamicsCaseStudy"; }
};

int main(int argc, char* argv[]) {
  using CaseStudy = ParallelUpdateDynamicsCaseStudyWithBlocking;
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

  elastica::Time time_v{0.0};
  elastica::TimeDelta dt{0.000002};
  elastica::TimeStepID id{1};  // TODO fix

  auto record_profiling_data = [&](auto& study) {
    // This is a hack around YamlCPP deficiences
    ::elastica::io::YAMLOutputArchive writer{
        metadata.write_at({time_v, dt, id})};
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

  elastica::TimeStepID final_id{id};
  const std::size_t n_iterations(10000UL);
  final_id.value() = n_iterations;

  std::size_t time_in_ns{0U};

  for (; id < final_id; ++id) {
    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();
    case_study.simulator->for_each_block_update_dynamics(
        [=](auto& system) noexcept {
          system.dynamic_states() += dt * system.dynamic_rates(time_v);
        });
    //    case_study.simulator->for_each_block_update_kinematics(
    //        [=](auto& system) noexcept {
    //          system.kinematic_states() += dt *
    //          system.kinematic_rates(time_v);
    //        });
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    time_in_ns +=
        (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)
             .count());
    time_v += dt;
  }
  ///
  record_profiling_data(case_study);

  std::cout << "Finished execution and wrote profiling information"
            << std::endl;
  std::cout << "Took " << double(time_in_ns) / 1e9 << " s" << std::endl;

  return EXIT_SUCCESS;
}
