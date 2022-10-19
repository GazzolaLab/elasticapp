//
// Created by Tejaswin Parthasarathy on 10/4/22.
//

#ifndef ELASTICA_SNAKEPIT_HPP
#define ELASTICA_SNAKEPIT_HPP

//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/ModuleSettings/Parallel.hpp>
#include <elastica/Simulator/Constraints.hpp>
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Forcing.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Geometry/Angles.hpp>
#include <elastica/Utilities/TMPL.hpp>
//
#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using CosseratRod = ::elastica::cosserat_rod::CosseratRodWithoutDamping;
using ImposedHalfSpace = ::elastica::collision::ImposedHalfSpacePrimitive;

struct SnakePitConfiguration {
  static inline auto make_simulator_configuration() {
    using EnvironmentTags =
        tmpl::list<::elastica::environments::tags::GravityField,
                   ::elastica::environments::tags::CollisionSystem>;
    using ImposedPrimitives = tmpl::list<ImposedHalfSpace>;

#if ELASTICA_TBB_PARALLEL_MODE
#define EXEC ec::par
#else
#define EXEC ec::seq
#endif

    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRod>>(
            ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{128UL}),
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(EXEC)));

    // use default force model of a linear spring damper
    auto collision_config = ec::make_collision_configuration(
        EXEC,      // between each type
        ec::seq);  // across each type (there is only one tpe)
    auto gravity_config =
        ec::make_gravity_configuration(::elastica::Vec3{0., 0., 0.});

    auto environment_config =
        ec::make_environments_configuration<EnvironmentTags, ImposedPrimitives>(
            // iteration, currently unused
            ec::seq, gravity_config, collision_config);

    auto constraints_config = ec::make_constraints_configuration<>(EXEC);
    auto forcing_config = ec::make_forcing_configuration<>(EXEC);

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{}, constraints_config, forcing_config,
        environment_config);
#undef EXEC
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Constraints,
                                    ::elastica::modules::Forcing,
                                    ::elastica::modules::Environments>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct SnakePit : public ::elastica::CaseStudyInterface<SnakePit> {
  using Configuration = SnakePitConfiguration;
  using This = SnakePit;

  static constexpr String help = {R"help(
TODO
)help"};

  struct n_elements {
    using type = std::size_t;
    static constexpr String help = {"Number of elements in the rod"};
    static type default_value() { return 20UL; }
  };

  struct youngs_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Youngs modulus of the rod"};
    static type default_value() { return 1e6; }
  };

  struct shear_modulus {
    using type = ::elastica::real_t;
    static constexpr String help = {"Shear modulus of the rod"};
    static type default_value() {
      // poisson ratio 0.5
      return youngs_modulus::default_value() / 1.5;
    }
  };

  struct density {
    using type = ::elastica::real_t;
    static constexpr String help = {"Density of the rod"};
    static type default_value() { return 1000.0; }
  };

  struct length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Length of the rod"};
    static type default_value() { return 0.35; }
  };

  struct radius {
    using type = ::elastica::real_t;
    static constexpr String help = {"Radius of the rod"};
    static type default_value() { return length::default_value() * 0.011; }
  };

  struct damping {
    using type = ::elastica::real_t;
    static constexpr String help = {"Internal damping in the rod"};
    static type default_value() { return 1e-4; }
  };

  struct contact_stiffness {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Stiffness in contact between the two rods"};
    static type default_value() { return 1e4; }
  };

  struct contact_damping {
    using type = ::elastica::real_t;
    static constexpr String help = {"Damping in contact between the two rods"};
    static type default_value() { return 1e-3; }
  };

  struct gravitational_acc {
    using type = ::elastica::real_t;
    static constexpr String help = {"Gravitational acceleration"};
    static type default_value() { return 0.0; }
  };

  struct n_snakes {
    using type = std::size_t;
    static constexpr String help = {"Number of snakes"};
    static type default_value() { return 4UL; }
  };

  struct time_period {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time period"};
    static type default_value() { return 2.0; }
  };

  struct wave_length {
    using type = ::elastica::real_t;
    static constexpr String help = {"Activation wave-length"};
    static type default_value() { return 1.0; }
  };

  struct activation_start_time_nd {
    using type = ::elastica::real_t;
    static constexpr String help = {"Non dimensional activation start time"};
    static type default_value() { return 5.0; }
  };

  struct packing_initial_vertical_span_ratio {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Ratio of Vertical span of the initialised packing to snake length"};
    static type default_value() { return 1.0; }
  };

  struct packing_initial_radial_span_ratio {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Ratio of Radial span of the initialised packing to snake length"};
    static type default_value() { return 1.0; }
  };

  struct samples_per_period {
    using type = std::size_t;
    static constexpr String help = {"Number of samples per time-period"};
    static type default_value() { return 32UL; }
  };

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() { return 5e-5; }
  };

  struct final_sim_time {
    using type = ::elastica::real_t;
    static constexpr String help = {"Final simulation time"};
    static type default_value() { return 10.0 * time_period::default_value(); }
  };

  using Parameters = tmpl::list<
      n_elements, youngs_modulus, shear_modulus, density, radius, length,
      damping, contact_stiffness, contact_damping, gravitational_acc, n_snakes,
      time_period, activation_start_time_nd, wave_length,
      packing_initial_vertical_span_ratio, packing_initial_radial_span_ratio,
      samples_per_period, final_sim_time, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  SnakePit() = default;
  SnakePit(SnakePit&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using CosseratRodRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  using CosseratRodRefs = std::vector<CosseratRodRef>;

  ::elastica::real_t final_time;
  ::elastica::real_t snake_time_period;
  std::size_t n_samples_per_period;
  CosseratRodRefs cosserat_rod_refs;

  using ImposedHalfSpaceRef =
      decltype(simulator->get_invalid_ref<ImposedHalfSpace>());
  ImposedHalfSpaceRef imposed_half_space_ref =
      simulator->get_invalid_ref<ImposedHalfSpace>();

  inline auto communicate_from(CosseratRodRef& ref) { return ref.get(); }
  inline auto communicate() {
    // we dont have CTAD :(
    using CT = decltype(communicate_from(cosserat_rod_refs[0UL]));
    using RT = std::vector<CT>;
    RT return_list;
    return_list.reserve(cosserat_rod_refs.size());
    std::transform(std::begin(cosserat_rod_refs), std::end(cosserat_rod_refs),
                   std::back_inserter(return_list),
                   [this](CosseratRodRef& rod_ref) {
                     return this->communicate_from(rod_ref);
                   });
    // return also the interpenetration here
    auto& collision_system =
        simulator
            ->get_environment<elastica::environments::tags::CollisionSystem>();
    auto& contacts = collision_system->contacts();
    using Penetrations = std::vector<elastica::real_t>;
    Penetrations penetrations;
    penetrations.reserve(contacts.size());
    std::transform(std::cbegin(contacts), std::cend(contacts),
                   std::back_inserter(penetrations),
                   [](auto& c) { return -c.get_distance(); });

#ifdef DOODO
    using ID = std::pair<std::size_t, std::size_t>;
    using IDs = std::vector<ID>;
    IDs contacting_primitive_ids;
    contacting_primitive_ids.reserve(contacts.size());
    std::transform(std::cbegin(contacts), std::cend(contacts),
                   std::back_inserter(contacting_primitive_ids),
                   [](::elastica::collision::Contact const& c) {
                     return std::make_pair(c.get_first_body()->getSystemID(),
                                           c.get_second_body()->getSystemID());
                   });

    using PrimitivePos = blaze::DynamicVector<::elastica::real_t>;
    using PrimitivePositions = std::vector<PrimitivePos>;
    PrimitivePositions contacting_primitive_pos;
    contacting_primitive_pos.reserve(contacts.size());

    // return also the interpenetration here
    auto& capsules =
        simulator->primitives<::elastica::collision::CapsuleFromRodPrimitive>();

    std::transform(
        std::cbegin(capsules), std::cend(capsules),
        std::back_inserter(contacting_primitive_pos),
        [](::elastica::collision::CapsuleFromRodPrimitive const& cap) {
          return cap.get_position();
        });

    using PrimitiveAxes = PrimitivePositions;
    PrimitiveAxes contacting_primitive_axis;
    contacting_primitive_axis.reserve(contacts.size());

    std::transform(std::cbegin(capsules), std::cend(capsules),
                   std::back_inserter(contacting_primitive_axis),
                   [](::elastica::collision::CapsuleFromRodPrimitive const& cap)
                       -> PrimitivePos {
                     // direction cast
                     return {cap.get_rotation_matrix()(2UL, 0UL),
                             cap.get_rotation_matrix()(2UL, 1UL),
                             cap.get_rotation_matrix()(2UL, 2UL)};
                   });

    using PrimitiveRadii = Penetrations;
    PrimitiveRadii contacting_primitive_radii;
    contacting_primitive_radii.reserve(contacts.size());

    std::transform(
        std::cbegin(capsules), std::cend(capsules),
        std::back_inserter(contacting_primitive_radii),
        [](::elastica::collision::CapsuleFromRodPrimitive const& cap) {
          return cap.get_radius();
        });

    using PrimitiveLengths = PrimitiveRadii;
    PrimitiveLengths contacting_primitive_lengths;
    contacting_primitive_lengths.reserve(contacts.size());

    std::transform(
        std::cbegin(capsules), std::cend(capsules),
        std::back_inserter(contacting_primitive_lengths),
        [](::elastica::collision::CapsuleFromRodPrimitive const& cap) {
          return cap.get_length();
        });

    return std::make_tuple(std::move(penetrations), std::move(return_list),
                           std::move(contacting_primitive_ids),
                           std::move(contacting_primitive_pos),
                           std::move(contacting_primitive_axis),
                           std::move(contacting_primitive_radii),
                           std::move(contacting_primitive_lengths));
#else
    return std::make_tuple(std::move(penetrations), std::move(return_list));
#endif
  }
};

ELASTICA_REGISTER_CASE_STUDY(SnakePit);

#endif  // ELASTICA_SNAKEPIT_HPP
