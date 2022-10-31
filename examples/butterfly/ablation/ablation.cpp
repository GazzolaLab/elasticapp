#define ELASTICA_DEFAULT_SYSTEM_INDEX_CHECK unchecked

#include <elastica/Simulator/Configuration/ModuleConfiguration/BlockingPolicyStreams.hpp>
#include <elastica/Simulator/IO/YAML.hpp>
#include <elastica/Simulator/Utilities/Serialize.hpp>
#include <elastica/Utilities/AppSupport/OptionsParser/OptionsParser.hpp>
#include <elastica/Utilities/StdHelpers.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include <examples/butterfly/ButterflyCaseStudy.hpp>
//
#include <memory>

struct ButterflyCaseStudyWithBlocking : public ButterflyCaseStudy {
  // Keep it caps for consistency
  struct BlockingPolicy {
    using element_type = ::elastica::configuration::detail::BlockingPolicyBase<
        ::elastica::configuration::detail::BlockingPolicyRegistrars>;
    using type = std::unique_ptr<element_type>;
    static constexpr Options::String help = {"The type of BlockingPolicy"};
  };

  using Parameters = tmpl::append<typename ButterflyCaseStudy::Parameters,
                                  tmpl::list<BlockingPolicy>>;

  void setup(InputTypes<ButterflyCaseStudyWithBlocking> const& params) {
    // Change the simulator blocking here
    // hack to workaround non-copyable unique pointers. Params are always
    // stored downstream so they dont go out of scope. They will get deleted
    // eventually, so dont double delete in constructor
    simulator->blocking_policy().reset(
        tuples::get<BlockingPolicy>(params).get());
    // Need to slice it here
    ButterflyCaseStudy::setup(tuples::apply<
                              typename ButterflyCaseStudy::Parameters>(
        [](auto const&... values) {
          using Type =
              tmpl::as_tagged_tuple<typename ButterflyCaseStudy::Parameters>;
          return Type(values...);
        },
        params));
  }

  ~ButterflyCaseStudyWithBlocking() {
    // release the pointer : it will get cleaned up with the downstream
    // destructor
    simulator->blocking_policy().release();
  }

  static std::string name() { return "ButterflyCaseStudy"; }
};

int main(int argc, char* argv[]) {
  using CaseStudy = ButterflyCaseStudyWithBlocking;
  char const* case_study_name("butterfly");

  auto case_study =
      ::standalone_bindings::parse_case_study_from_options<CaseStudy>(
          case_study_name, argc, argv);
  // std::cout << "Setup case-study" << std::endl;
  // std::cout << case_study.parameters() << std::endl;

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

  record_profiling_data(case_study);
  // Don't write any output information
  case_study.run_for_n_iterations(static_cast<std::size_t>(
      case_study.final_time / case_study.current_dt()));
  // case_study.run();
  record_profiling_data(case_study);

  std::cout << "Finished execution and wrote profiling information"
            << std::endl;

  return EXIT_SUCCESS;
}
