//
// Created by Tejaswin Parthasarathy on 11/7/22.
//

#pragma once

#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
//
#include <elastica/Utilities/Math/Dot.hpp>
#include <elastica/Utilities/Math/Length.hpp>
#include <elastica/Utilities/Math/Vec3.hpp>
//
#include <cmath>

/*
 This function takes the velocity of elements and checks if they are larger than
 the threshold velocity. If the velocity of elements is larger than threshold
 velocity, that means those elements are slipping. In other words, kinetic
 friction will be acting on those elements, not static friction. This function
 outputs an array called slip function, this array has a size of the number of
 elements. If the velocity of the element is smaller than the threshold velocity
 slip function value for that element is 1, which means static friction is
 acting on that element. If the velocity of the element is larger than the
 threshold velocity slip function value for that element is between 0 and 1,
 which means kinetic friction is acting on that element.

 Parameters
 ----------
 velocity_slip : numpy.ndarray
     2D (dim, blocksize) array containing data with 'float' type.
     Rod-like object element velocity.
 velocity_threshold : float
     Threshold velocity to determine slip.

 Returns
 -------
 slip_function : numpy.ndarray
     2D (dim, blocksize) array containing data with 'float' type.
*/
inline elastica::real_t slip_function(
    elastica::Vec3 const& velocity_slip,
    elastica::real_t const velocity_threshold) {
  auto const abs_velocity_slip = elastica::length(velocity_slip);
  return (abs_velocity_slip > velocity_threshold)
             ? std::abs(
                   1.0 -
                   std::min(1.0, abs_velocity_slip / velocity_threshold - 1.0))
             : 1.0;
}

template <typename CRT,
          template <typename /*CRT*/, typename /*InitializedBlock*/>
          class... Components>
void calculate_force(
    blocks::BlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
        CRT, blocks::BlockSlice, Components...>>& system,
    elastica::real_t const k, elastica::real_t const nu,
    elastica::Vec3 const plane_origin, elastica::Vec3 const plane_normal,
    elastica::real_t const surface_tol,
    elastica::real_t const slip_velocity_tol, elastica::real_t const kinetic_mu,
    elastica::real_t const static_mu) {
  //  auto& system = block_like.self();
  auto const n_elem = cpp17::size(system);

  // std::cout << system.get_internal_loads() << std::endl;

  for (auto elem_idx = 0UL; elem_idx < n_elem; ++elem_idx) {
    elastica::Vec3 const element_position =
        0.5 *
        (system.get_position(elem_idx) + system.get_position(elem_idx + 1UL));
    elastica::real_t const distance_from_plane(
        elastica::dot(plane_normal, element_position - plane_origin));
    elastica::real_t const potential_penetration(
        distance_from_plane - 0.5 * system.get_element_length(elem_idx));

    // Cull soon, cull hard
    if (potential_penetration > surface_tol) {
      continue;
    }

    elastica::Vec3 total_forces_at_idx = system.get_internal_loads(elem_idx) +
                                         system.get_external_loads(elem_idx);
    elastica::Vec3 total_forces_at_next_idx =
        system.get_internal_loads(elem_idx + 1UL) +
        system.get_external_loads(elem_idx + 1UL);
    elastica::Vec3 element_total_forces =
        0.5 * (total_forces_at_idx + total_forces_at_next_idx);
    if (elem_idx == 0)
      element_total_forces += 0.5 * total_forces_at_idx;
    if (elem_idx == (n_elem - 1))
      element_total_forces += 0.5 * total_forces_at_next_idx;

    elastica::real_t const force_component_along_normal_direction(
        elastica::dot(element_total_forces, plane_normal));

    // If the total force component along the plane normal direction is greater
    // than zero that means, total force is pushing rod away from the plane not
    // towards the plane. Thus, response force applied by the surface has to be
    // zero.

    // Compute response force on the element. Plane response force
    // has to be away from the surface and towards the element. Thus
    // multiply forces along normal direction with negative sign.
    // DOES NOT MATCH PYELASTICA: Matches first x time-steps after which the
    // trajectories deviate

    auto const mag(force_component_along_normal_direction > 0.0
                       ? 0.0
                       : -force_component_along_normal_direction);
    elastica::Vec3 const plane_response_force(
        // -std::min(0.0, force_component_along_normal_direction)
        mag * plane_normal);

    // why is this needed again?
    elastica::real_t const plane_penetration =
        std::min(potential_penetration, 0.0);
    elastica::Vec3 const elastic_force(-(k * plane_penetration) * plane_normal);

    // Damping force response due to velocity towards the plane
    elastica::Vec3 const element_velocity =
        (system.get_mass(elem_idx) * system.get_velocity(elem_idx) +
         system.get_mass(elem_idx + 1UL) *
             system.get_velocity(elem_idx + 1UL)) /
        (system.get_mass(elem_idx) + system.get_mass(elem_idx + 1UL));
    elastica::real_t const normal_component_of_element_velocity(
        elastica::dot(element_velocity, plane_normal));
    elastica::Vec3 const damping_force(
        -(nu * normal_component_of_element_velocity) * plane_normal);

    // Compute total plane response force
    elastica::Vec3 const half_plane_response_force_total =
        (elastic_force +
         damping_force);  // plane_response_force +  + damping_force);

    //    if (potential_penetration < surface_tol) {
    system.get_external_loads(elem_idx) +=
        0.5 * half_plane_response_force_total;
    system.get_external_loads(elem_idx + 1UL) +=
        0.5 * half_plane_response_force_total;
    //    }

    /////////////////////////////////////////////////////////////////////
#define KINETIC_FRICTION_ENABLED 1
#define STATIC_FRICTION_ENABLED 1
#define FRICTION_ENABLED KINETIC_FRICTION_ENABLED || STATIC_FRICTION_ENABLED

#if FRICTION_ENABLED
    elastica::real_t const plane_response_force_mag =
        elastica::length(half_plane_response_force_total);
    elastica::Vec3 const axial_direction(system.get_tangent(elem_idx));
    //    elastica::Vec3 const axial_direction{1.0, 0.0, 0.0};

    if (std::abs(elastica::length(axial_direction) - 1.0) > 1e-8)
      throw std::runtime_error("What");

    elastica::real_t const velocity_mag_along_axial_direction(
        elastica::dot(element_velocity, axial_direction));
    elastica::Vec3 const velocity_along_axial_direction(
        velocity_mag_along_axial_direction * axial_direction);
    elastica::Vec3 const velocity_perpendicular_to_axial_direction(
        element_velocity - velocity_along_axial_direction);

    // Friction forces depends on the direction of velocity, in other words sign
    // of the velocity vector.
    // Call slip function to check if elements slipping or not
    elastica::real_t const slip_function_perpendicular_to_axial_direction =
        slip_function(velocity_perpendicular_to_axial_direction,
                      slip_velocity_tol);
    //    elastica::real_t const
    //    slip_function_perpendicular_to_axial_direction(0.1);

    const real_t l =
        elastica::length(velocity_perpendicular_to_axial_direction);
    elastica::Vec3 const unitized_total_velocity_in_plane(
        l < 1e-8 ? elastica::Vec3{0., 0., 0.}
                 : velocity_perpendicular_to_axial_direction / l);

    // Apply kinetic friction in axial direction.
    elastica::Vec3 const
        kinetic_friction_force_perpendicular_to_axial_direction =
            -((1.0 - slip_function_perpendicular_to_axial_direction) *
              kinetic_mu * plane_response_force_mag *
              unitized_total_velocity_in_plane);
    //    std::cout <<  element_velocity << velocity_along_axial_direction <<
    //    std::endl;

#if KINETIC_FRICTION_ENABLED
    system.get_external_loads(elem_idx) +=
        0.5 * kinetic_friction_force_perpendicular_to_axial_direction;
    system.get_external_loads(elem_idx + 1UL) +=
        0.5 * kinetic_friction_force_perpendicular_to_axial_direction;
#endif

    // Need to recompute elemental_total_forces again after addition to buffer
    total_forces_at_idx = system.get_internal_loads(elem_idx) +
                          system.get_external_loads(elem_idx);
    total_forces_at_next_idx = system.get_internal_loads(elem_idx + 1UL) +
                               system.get_external_loads(elem_idx + 1UL);
    element_total_forces =
        0.5 * (total_forces_at_idx + total_forces_at_next_idx);
    if (elem_idx == 0)
      element_total_forces += 0.5 * total_forces_at_idx;
    if (elem_idx == (n_elem - 1))
      element_total_forces += 0.5 * total_forces_at_next_idx;

    // static friction
    elastica::real_t const force_component_along_axial_direction(
        elastica::dot(element_total_forces, axial_direction));

    // OLD VERSION IS ALSO WORKING
#ifdef OLD_VERSION
    elastica::Vec3 const force_component_perpendicular_to_axial_direction(
        element_total_forces -
        force_component_along_axial_direction * elastica::Vec3{1.0, 1.0, 1.0});
#else
    elastica::Vec3 const force_component_perpendicular_to_axial_direction(
        element_total_forces -
        force_component_along_axial_direction * axial_direction);
#endif

    const real_t mmag = elastica::length(
        force_component_perpendicular_to_axial_direction + 1e-14);
    elastica::Vec3 const static_force_direction(
        mmag < 1e-8 ? elastica::Vec3{0., 0., 0.}
                    : force_component_perpendicular_to_axial_direction / mmag);

    //    elastica::Vec3 const static_force_direction(
    //        force_component_perpendicular_to_axial_direction /
    //        elastica::length(force_component_perpendicular_to_axial_direction
    //        +
    //                         1e-14 * elastica::Vec3{1.0, 1.0, 1.0}));

#ifdef OLD_VERSION
    elastica::Vec3 const force_component_sign_perpendicular_to_axial_direction{
        force_component_perpendicular_to_axial_direction[0UL] < 0.0 ? -1.0
                                                                    : 1.0,
        force_component_perpendicular_to_axial_direction[1UL] < 0.0 ? -1.0
                                                                    : 1.0,
        force_component_perpendicular_to_axial_direction[2UL] < 0.0 ? -1.0
                                                                    : 1.0,
    };
#else
    elastica::real_t const
        force_component_sign_perpendicular_to_axial_direction(
            elastica::dot(force_component_perpendicular_to_axial_direction,
                          static_force_direction) < 0
                ? -1.0
                : 1.0);
#endif

    elastica::real_t const max_friction_force(
        slip_function_perpendicular_to_axial_direction * static_mu *
        plane_response_force_mag);

#ifdef OLD_VERSION
    elastica::Vec3 const
        static_friction_force_perpendicular_to_axial_direction = -(
            elastica::Vec3{
                std::min(
                    std::abs(
                        force_component_perpendicular_to_axial_direction[0UL]),
                    max_friction_force),
                std::min(
                    std::abs(
                        force_component_perpendicular_to_axial_direction[1UL]),
                    max_friction_force),
                std::min(
                    std::abs(
                        force_component_perpendicular_to_axial_direction[2UL]),
                    max_friction_force),
            } *
            force_component_sign_perpendicular_to_axial_direction *
            static_force_direction);
#else
    auto const kl =
        elastica::length(force_component_perpendicular_to_axial_direction);
    elastica::Vec3 const
        static_friction_force_perpendicular_to_axial_direction =
            -(std::min(kl, max_friction_force) *
              force_component_sign_perpendicular_to_axial_direction *
              static_force_direction);

#endif

#if STATIC_FRICTION_ENABLED
    system.get_external_loads(elem_idx) +=
        0.5 * static_friction_force_perpendicular_to_axial_direction;
    system.get_external_loads(elem_idx + 1UL) +=
        0.5 * static_friction_force_perpendicular_to_axial_direction;
#endif  // STATIC_FRICTION
#endif  // FRICTION
  }     // elem_idx

#undef KINETIC_FRICTION_ENABLED
#undef STATIC_FRICTION_ENABLED
#undef FRICTION_ENABLED
}
