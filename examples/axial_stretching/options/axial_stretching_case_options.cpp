#include <elastica/Simulator/Configuration/ModuleConfiguration/BlockingPolicyStreams.hpp>
#include <elastica/Simulator/IO/YAML.hpp>
#include <elastica/Simulator/Utilities/Serialize.hpp>
#include <elastica/Utilities/AppSupport/OptionsParser/OptionsParser.hpp>
// ?
#include "examples/axial_stretching/AxialStretchingCaseStudy.hpp"
//
#include <memory>

// This is a hack to not affect the code for AxialStretchingCaseStudy
// For other case-studies, we can inline this into the study itself
struct BlockingPolicy {
  using element_type = ::elastica::configuration::detail::BlockingPolicyBase<
      ::elastica::configuration::detail::BlockingPolicyRegistrars>;
  using type = std::unique_ptr<element_type>;
  static constexpr Options::String help = {"The type of BlockingPolicy"};
};

struct AxialStretchingCaseStudyWithBlocking : public AxialStretchingCaseStudy {
  using Parameters =
      tmpl::push_back<typename AxialStretchingCaseStudy::Parameters,
                      BlockingPolicy>;

  void setup(InputTypes<AxialStretchingCaseStudyWithBlocking> const& params) {
    // Change the simulator blocking here
    // hack to workaround non-copyable unique pointers. Params are always
    // stored  downstream so they dont go out of scope. They will get deleted
    // eventually, so remember to not double delete
    //    //
    //    auto* p = simulator->blocking_policy().get();
    //    p = tuples::get<BlockingPolicy>(params).get();

    simulator->blocking_policy().reset(
        tuples::get<BlockingPolicy>(params).get());

    // Forward to AxialStretchingSetup after slicing blocking policy out
    AxialStretchingCaseStudy::setup(
        tuples::apply<typename AxialStretchingCaseStudy::Parameters>(
            [](auto const&... values) {
              return InputTypes<AxialStretchingCaseStudy>(values...);
            },
            params));
  }

  ~AxialStretchingCaseStudyWithBlocking() {
    // release the pointer : it will get cleaned up with the downstream
    // destructor
    simulator->blocking_policy().release();
  }
};

int main(int argc, char* argv[]) {
  using CaseStudy = AxialStretchingCaseStudyWithBlocking;
  char const* case_study_name("example_axial_stretching");

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
    writer(::elastica::io::make_nvp("profiling", study.simulator->timers()));
  };

  record_profiling_data(case_study);
  //  Use case-study interface such as run() and all its members
  case_study.run_for_n_iterations(100UL);
  record_profiling_data(case_study);
  std::cout << "Finished execution and wrote profiling information"
            << std::endl;

  return EXIT_SUCCESS;
}
