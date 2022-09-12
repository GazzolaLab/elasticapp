//==============================================================================
/*!
//  \file
//  \brief Muscle RL Environment
//
//  Copyright (C) 2022 Seung Hyun Kim - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

#include <elastica/Simulator/Connections.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math/Dot.hpp>
#include <elastica/Utilities/Math/Normalize.hpp>
#include <iostream>
#include <tuple>

namespace econ = ::elastica::connections;

struct CustomConnection {
  using real_t = ::elastica::real_t;
  using Vec3 = ::elastica::Vec3;
  using Traits = ::elastica::cosserat_rod::DefaultCosseratRodTraits;

  using Scalar = Traits::DataType::Scalar;
  using Vector = Traits::DataType::Vector;
  using Matrix = Traits::DataType::Matrix;
  using TOp = Traits::Operations;

  template <typename RodSliceTypeA, typename RodSliceTypeB>
  static auto get_connection_vector_straight_straight_rod(
      RodSliceTypeA const& rod_one, RodSliceTypeB const& rod_two,
      const std::size_t rod_one_start_idx, const std::size_t rod_one_end_idx,
      const std::size_t rod_two_start_idx, const std::size_t rod_two_end_idx) {
    assert(rod_one_end_idx - rod_one_start_idx ==
           rod_two_end_idx - rod_one_end_idx);

    const std::size_t n_elem = rod_one_end_idx - rod_one_start_idx;

    /* get Position vector */
    blaze::DynamicMatrix<real_t> rod_one_element_position = Vector::slice(
        TOp::average_kernel(rod_one.get_position()), rod_one_start_idx, n_elem);
    blaze::DynamicMatrix<real_t> rod_two_element_position = Vector::slice(
        TOp::average_kernel(rod_two.get_position()), rod_two_start_idx, n_elem);

    /* get Director vector */
    blaze::DynamicTensor<real_t> rod_one_director =
        Matrix::slice(::elastica::director(rod_one), rod_one_start_idx, n_elem);
    blaze::DynamicTensor<real_t> rod_two_director =
        Matrix::slice(::elastica::director(rod_two), rod_two_start_idx, n_elem);

    /* get Radius vector */
    blaze::DynamicVector<real_t> rod_one_radius = Scalar::slice(
        ::blocks::get<::elastica::tags::ElementDimension>(rod_one),
        rod_one_start_idx, n_elem);
    blaze::DynamicVector<real_t> rod_two_radius = Scalar::slice(
        ::blocks::get<::elastica::tags::ElementDimension>(rod_two),
        rod_two_start_idx, n_elem);

    /* get rod-to-rod vector */
    blaze::DynamicMatrix<real_t> position_diff =
        rod_two_element_position - rod_one_element_position;
    blaze::DynamicVector<real_t> distance_vector_rod_one_to_rod_two_norm =
        Traits::Operations::batch_norm(position_diff);
    blaze::DynamicMatrix<real_t> distance_vector_rod_one_to_rod_two(3UL,
                                                                    n_elem);
    Traits::Operations::batch_division_matvec(
        distance_vector_rod_one_to_rod_two, position_diff,
        distance_vector_rod_one_to_rod_two_norm);
    blaze::DynamicMatrix<real_t> distance_vector_rod_two_to_rod_one(
        -distance_vector_rod_one_to_rod_two);

    /* get direction vector in material frame */
    blaze::DynamicMatrix<real_t> rod_one_direction_vec_in_material_frame(
        3UL, n_elem);
    blaze::DynamicMatrix<real_t> rod_two_direction_vec_in_material_frame(
        3UL, n_elem);
    Traits::Operations::batch_matvec(rod_one_direction_vec_in_material_frame,
                                     rod_one_director,
                                     distance_vector_rod_one_to_rod_two);
    Traits::Operations::batch_matvec(rod_two_direction_vec_in_material_frame,
                                     rod_two_director,
                                     distance_vector_rod_two_to_rod_one);
    blaze::DynamicVector<real_t> offset_btw_rods =
        distance_vector_rod_one_to_rod_two_norm -
        (rod_one_radius + rod_two_radius);

    return std::make_tuple(rod_one_direction_vec_in_material_frame,
                           rod_two_direction_vec_in_material_frame,
                           offset_btw_rods);
  }

  static auto parallel_connection_generator(
      const real_t k, const real_t nu, const real_t k_repulsive,
      const Vec3 rod_one_direction_vec_in_material_frame,
      const Vec3 rod_two_direction_vec_in_material_frame,
      const real_t offset_btw_rods) {
    /* element-wise surface side-by-side connection */
    auto element_wise_boundary = [k = k, nu = nu, k_repulsive = k_repulsive,
                                  rest_offset_btw_rods = offset_btw_rods,
                                  rod_one_direction_vec_in_material_frame =
                                      rod_one_direction_vec_in_material_frame,
                                  rod_two_direction_vec_in_material_frame =
                                      rod_two_direction_vec_in_material_frame](
                                     auto /*time*/, auto& first_rod,
                                     std::size_t arg_first_index,
                                     auto& second_rod,
                                     std::size_t arg_second_index) {
      //      const auto first_index = arg_first_index + 1UL;
      //      const auto second_index = arg_second_index;

      Vec3 rod_one_to_rod_two_connection_vec =
          Traits::Operations::transpose(
              ::elastica::director(first_rod, arg_first_index)) *
          rod_one_direction_vec_in_material_frame;
      Vec3 rod_two_to_rod_one_connection_vec =
          Traits::Operations::transpose(
              ::elastica::director(second_rod, arg_second_index)) *
          rod_two_direction_vec_in_material_frame;

      // Compute element positions
      Vec3 rod_one_element_position =
          0.5 * (::elastica::position(first_rod, arg_first_index) +
                 ::elastica::position(first_rod, arg_first_index + 1));
      Vec3 rod_two_element_position =
          0.5 * (::elastica::position(second_rod, arg_second_index) +
                 ::elastica::position(second_rod, arg_second_index + 1));

      // If there is an offset between rod one and rod two surface, then it
      // should change as a function of dilatation.
      real_t offset_rod_one = rest_offset_btw_rods * 0;
      real_t offset_rod_two =
          -second_rod.get_element_dimension(arg_second_index);
      real_t end_point_flag = rest_offset_btw_rods / (rest_offset_btw_rods);
      Vec3 rod_one_rd2 = rod_one_to_rod_two_connection_vec *
                         first_rod.get_element_dimension(arg_first_index);
      Vec3 rod_two_rd2 = rod_two_to_rod_one_connection_vec * 0;

      // Compute connection points on the rod surfaces
      Vec3 surface_position_rod_one = rod_one_element_position + rod_one_rd2;
      Vec3 surface_position_rod_two = rod_two_element_position + rod_two_rd2;

      // Compute spring force between two rods
      Vec3 distance_vector =
          surface_position_rod_two - surface_position_rod_one;
      Vec3 spring_force = k * (distance_vector);
      //      std::cout << "sp: " << spring_force << '\n';

      Vec3 projection_vec = blaze::trans(
          blaze::row(::elastica::director(first_rod, arg_first_index), 2UL));
      Vec3 project_force = blaze::dot(spring_force, projection_vec) /
                           blaze::norm(projection_vec) * projection_vec;
      Vec3 modified_spring_force =
          spring_force - (project_force * (1 - end_point_flag));

      // Damping force
      // real_t damping_force(-nu);

      // Compute contact forces. Contact forces are applied in the case one rod
      // penetrates to the other, in that case we apply a repulsive force. Later
      // on these repulsive forces are used to move rods apart from each other
      // and as a pressure force. We assume contact forces are in plane.

      Vec3 center_distance =
          rod_two_element_position - rod_one_element_position;
      real_t center_distance_norm = blaze::norm(center_distance);
      Vec3 center_distance_unit_vec = center_distance / center_distance_norm;
      real_t penetration_strain =
          center_distance_norm /
              (first_rod.get_element_dimension(arg_first_index) +
               offset_rod_one +
               second_rod.get_element_dimension(arg_second_index) +
               offset_rod_two) -
          1;
      // round_(penetration_strain, 12, penetration_strain)
      real_t is_penetrated = static_cast<real_t>(penetration_strain < 0);
      real_t k_contact = is_penetrated * k_repulsive *
                         blaze::pow(blaze::abs(penetration_strain), 1.5);
      Vec3 contact_force = k_contact * center_distance_unit_vec;

      // Compute the spring forces in plane. If there is contact spring force is
      // also contributing to contact force so we need to compute it and add to
      // contact_force.
      Vec3 spring_force_temp_for_contact = is_penetrated * spring_force;
      contact_force += spring_force_temp_for_contact;

      // Compute the total force
      Vec3 total_force =
          modified_spring_force + contact_force;  // + damping_force

      // Add total forces on external load
      using LoadTag = ::elastica::tags::ExternalLoads;
      blaze::column(::blocks::get<LoadTag>(first_rod), arg_first_index) +=
          0.5 * total_force;
      blaze::column(::blocks::get<LoadTag>(first_rod), arg_first_index + 1) +=
          0.5 * total_force;
      blaze::column(::blocks::get<LoadTag>(second_rod), arg_second_index) -=
          0.5 * total_force;
      blaze::column(::blocks::get<LoadTag>(second_rod), arg_second_index + 1) -=
          0.5 * total_force;

      /* Make Torque */
      // Compute torques due to the connection forces

      Vec3 torque_on_rod_one = rod_one_rd2 % modified_spring_force;
      Vec3 torque_on_rod_two = rod_two_rd2 % (-modified_spring_force);

      using TorqueTag = ::elastica::tags::ExternalTorques;
      // this is not a nodal quantitiy
      blaze::column(::blocks::get<TorqueTag>(first_rod), arg_first_index) +=
          (::elastica::director(first_rod, arg_first_index) *
           torque_on_rod_one);
      blaze::column(::blocks::get<TorqueTag>(second_rod), arg_second_index) +=
          (::elastica::director(second_rod, arg_second_index) *
           torque_on_rod_two);
    };

    /* spanwise surface side-by-side connection */
    auto spanwise_boundary = [=](auto /*time*/, auto& first_rod,
                                 auto& second_rod) {};

    return econ::make_connection(element_wise_boundary, spanwise_boundary);
  };
};