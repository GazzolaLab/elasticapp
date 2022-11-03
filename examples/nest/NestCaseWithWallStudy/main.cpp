#include "NestCaseWithWallStudy.hpp"
// Parallelism headers should already be included
#include <elastica/Simulator/IO/YAML.hpp>
#include <elastica/Simulator/Utilities/Serialize.hpp>
#include <elastica/Utilities/AppSupport/CaseStudyParser.hpp>
#include <elastica/ErrorHandling/FloatingPointExceptions.hpp>
//
#include <tbb/global_control.h>

int main(int argc, char* argv[]) {
    using CaseStudy = NestCaseWithWallStudy;
    char const* case_study_name("example_nest_with_wall_scaling");

    auto input_parameters =
    ::standalone_bindings::parse_case_study_parameters<CaseStudy>(case_study_name, argc, argv);

    // Execution profile : control number of threads here
    tbb::global_control application_limit(
      tbb::global_control::max_allowed_parallelism,
      tuples::get<typename CaseStudy::n_threads>(input_parameters));

    disable_floating_point_exceptions();
    CaseStudy case_study{};
    case_study.setup(input_parameters);

    ::elastica::io::YAMLMetadata metadata{};

    auto record_profiling_data = [&metadata](auto& study) {
      // This is a hack around YamlCPP deficiences
      ::elastica::io::YAMLOutputArchive writer{
  	metadata.write_at({study.get_current_time(), study.get_dt(),
  			   study.get_current_iteration()})};
      using ::elastica::for_each;
      writer(::elastica::io::make_nvp("profiling", study.simulator->timers()));
      writer(::elastica::io::make_nvp("environment_profiling", study.simulator->environment_timers()));
      auto& collision_system =
  	study.simulator
  	    ->template get_environment<elastica::environments::tags::CollisionSystem>();
      writer(::elastica::io::make_nvp("collision_system_profiling", collision_system->timers()));
      // Reset after every write.
      for_each(study.simulator->timers(), [](auto& tim) { tim.reset(); });
      for_each(collision_system->timers(), [](auto& tim) { tim.reset(); });
    };

    record_profiling_data(case_study);
    case_study.run_for_n_iterations(10000UL);
    record_profiling_data(case_study);

    return EXIT_SUCCESS;
}
    
