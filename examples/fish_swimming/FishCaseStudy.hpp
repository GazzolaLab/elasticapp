#pragma once
//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Connections.hpp>
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Time/Time.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/TMPL.hpp>
//
#include <examples/fish_swimming/FishModel/FishModel.hpp>
//
#include <algorithm>  // transform
#include <tuple>      // make_tuple
#include <vector>

using String = const char* const;
namespace ec = elastica::configuration;
using RodType = ::fish::Fish;
using ::elastica::real_t;
using ::elastica::Vec3;

struct FishConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config = ec::make_systems_configuration<tmpl::list<RodType>>(
        ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
        ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                                  ec::iterate_over_systems_in(ec::seq)));

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{},
        ec::DefaultConstraintsConfiguration{},
        ec::DefaultConnectionsConfiguration{},
        ec::DefaultForcingConfiguration{});
  }
  static inline auto make_simulator() {
    return ::elastica::make_simulator<::elastica::modules::Constraints,
                                      ::elastica::modules::Connections,
                                      ::elastica::modules::Forcing>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct FishCaseStudy : public ::elastica::CaseStudyInterface<FishCaseStudy> {
  using Configuration = FishConfiguration;
  using This = FishCaseStudy;

  static constexpr String help = {"Fish case study"};

  struct num_fish {
    using type = std::size_t;
    static constexpr String help = {"Number of fish"};
    static constexpr type default_value() { return 1UL; }
  };

  struct origins {
    using type = ::blaze::DynamicMatrix<real_t>;  // 3 x num_fish
    static constexpr String help = {"Origins of the fish"};
    static type default_value() { return {1UL, 3UL, 0.0}; }
  };

  struct n_elements {
    using type = std::size_t;
    static constexpr String help = {"Number of elements in the rod"};
    static constexpr type default_value() { return 50UL; }
  };

  struct density {
    using type = real_t;
    static constexpr String help = {"Density of the rod"};
    static constexpr type default_value() { return 66.0; }
  };

  struct youngs_modulus {
    using type = real_t;
    static constexpr String help = {"Youngs modulus of the rod"};
    static constexpr type default_value() { return 1.5e6; }
  };

  struct damping_rate {
    using type = real_t;
    static constexpr String help = {"Rate of damping in the rod"};
    static constexpr type default_value() { return 0.02; }
  };

  struct length {
    using type = real_t;
    static constexpr String help = {"Length of the rod"};
    static constexpr type default_value() { return 1.0; }
  };

  struct dt_scale {
    using type = real_t;
    static constexpr String help = {"Scale for time steps"};
    static constexpr type default_value() { return 5e-4; }
  };

  struct period {
    using type = real_t;
    static constexpr String help = {"Period of the rod's motion"};
    static constexpr type default_value() { return 1.0; }
  };

  using Parameters =
      tmpl::list<num_fish, origins, n_elements, damping_rate, density,
                 youngs_modulus, length, dt_scale, period>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  /* Python communication function */
  template <typename T>
  inline auto communicate_buffer(
      std::vector<blocks::BlockSliceGenerator<T>> const& ref_generators) {
    using CT =
        decltype(std::declval<blocks::BlockSliceGenerator<T> const&>().get());
    using RT = std::vector<CT>;
    RT return_list;
    return_list.reserve(ref_generators.size());
    std::transform(std::begin(ref_generators), std::end(ref_generators),
                   std::back_inserter(return_list),
                   [](blocks::BlockSliceGenerator<T> const& rod_ref) {
                     return rod_ref.get();
                   });
    return return_list;
  }

  inline auto communicate() {
    auto flow_force = ::blaze::submatrix(
        flow_forces, 0UL, 0UL, flow_forces.rows(), flow_forces.columns());
    auto flow_torque = ::blaze::submatrix(
        flow_torques, 0UL, 0UL, flow_torques.rows(), flow_torques.columns());
    return std::make_tuple(communicate_buffer(elastic_fish),
                           communicate_buffer(virtual_fish), flow_force,
                           flow_torque);
  }

  FishCaseStudy() = default;
  FishCaseStudy(FishCaseStudy const&) = default;
  FishCaseStudy(FishCaseStudy&&) noexcept = default;
  FishCaseStudy& operator=(FishCaseStudy const&) = default;
  FishCaseStudy& operator=(FishCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();

  // Class members
  using RodRef = decltype(simulator->get_invalid_ref<RodType>());
  std::vector<RodRef> elastic_fish;
  std::vector<RodRef> virtual_fish;

  ::blaze::DynamicMatrix<real_t> flow_forces;
  ::blaze::DynamicMatrix<real_t> flow_torques;
};

ELASTICA_REGISTER_CASE_STUDY(FishCaseStudy);
