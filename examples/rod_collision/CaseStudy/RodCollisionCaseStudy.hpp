//==============================================================================
/*!
//  \file
//  \brief Elastic-collision case study between two rods
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#ifndef ELASTICA_RODCOLLISIONCASESTUDY_HPP
#define ELASTICA_RODCOLLISIONCASESTUDY_HPP

//
#include <elastica/CaseStudy.hpp>
//
#include <elastica/Simulator/Environments.hpp>
#include <elastica/Simulator/Simulator.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Geometry/Angles.hpp>
#include <elastica/Utilities/TMPL.hpp>

// global is fine since its a cpp file
using String = const char* const;
namespace ec = elastica::configuration;
using ::elastica::cosserat_rod::CosseratRod;

struct RodCollisionConfiguration {
  static inline auto make_simulator_configuration() {
    auto system_config =
        ec::make_systems_configuration<tmpl::list<CosseratRod>>(
            ec::DefaultBlockingPolicy{},
            // sequential
            ec::make_iteration_policy(
                ec::iterate_across_system_types_in(ec::seq),
                ec::iterate_over_systems_in(ec::seq)));

    auto environment_config = ec::make_environments_configuration<
        tmpl::list<elastica::environments::tags::CollisionSystemTag>>(
        // iteration, currently unused
        ec::seq,
        // use default force model of a linear spring damper
        ec::make_collision_configuration(ec::seq, ec::seq));

    return ec::make_simulator_configuration(
        ec::DefaultDomainConfiguration{}, system_config,
        ec::DefaultParallelConfiguration{}, environment_config);
  }

  static inline auto make_simulator() {
    return elastica::make_simulator<::elastica::modules::Environments>(
        make_simulator_configuration());
  }

  static inline auto make_time_stepper() {
    using TimeStepper = ::elastica::timestepper::PositionVerlet;
    return TimeStepper{};
  }
};

struct RodCollisionCaseStudy
    : public ::elastica::CaseStudyInterface<RodCollisionCaseStudy> {
  using Configuration = RodCollisionConfiguration;
  using This = RodCollisionCaseStudy;

  static constexpr String help = {R"help(
Rod collision test-case
-----------------------

Runs an example of two rods, initially inclined wrt to one another, with some
initial velocity. These two eventually meet and forces generate because of
collision. No other forces (like gravity) are considered.

The collision forces generated are elastic, hence the total energy of the system
(kinetic energies + potential energy due to collision + elastic energies) should
conserved.

)help"};

  struct angle_of_inclination {
    using type = ::elastica::real_t;
    static constexpr String help = {
        "Angle of inclination between the two rods"};
    static type default_value() { return ::elastica::geometry::deg2rad(30.0); }
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

  struct dt {
    using type = ::elastica::real_t;
    static constexpr String help = {"Time step of simulation"};
    static type default_value() { return 5e-4; }
  };

  using Parameters =
      tmpl::list<angle_of_inclination, contact_stiffness, contact_damping, dt>;

  /* Required functions */
  void setup(InputTypes<This> const& input_parameters);
  void step();
  void run();

  RodCollisionCaseStudy() = default;
  RodCollisionCaseStudy(RodCollisionCaseStudy&&) noexcept = default;

  decltype(Configuration::make_time_stepper()) time_stepper =
      Configuration::make_time_stepper();
  decltype(Configuration::make_simulator()) simulator =
      Configuration::make_simulator();
  using CosseratRodRef = decltype(simulator->get_invalid_ref<CosseratRod>());
  using CosseratRodRefs = std::vector<CosseratRodRef>;

  CosseratRodRefs cosserat_rod_refs{2UL,
                                    simulator->get_invalid_ref<CosseratRod>()};

  inline auto communicate_from(CosseratRodRef& ref) {
    auto cosserat_rod = ref.get();
    return std::make_tuple(::elastica::position(cosserat_rod),
                           ::elastica::velocity(cosserat_rod),
                           ::elastica::external_loads(cosserat_rod));
  }
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
    auto& collision_system = simulator->get_environment<
        elastica::environments::tags::CollisionSystemTag>();
    auto& contacts = collision_system->contacts();
    using Penetrations = std::vector<elastica::real_t>;
    Penetrations penetrations;
    penetrations.reserve(contacts.size());
    std::transform(std::cbegin(contacts), std::cend(contacts),
                   std::back_inserter(penetrations),
                   [](auto& c) { return -c.get_distance(); });

#ifdef ELASTICA_DEBUG
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

ELASTICA_REGISTER_CASE_STUDY(RodCollisionCaseStudy);

#endif  // ELASTICA_RIGIDBODYCOLLISIONCASESTUDY_HPP
