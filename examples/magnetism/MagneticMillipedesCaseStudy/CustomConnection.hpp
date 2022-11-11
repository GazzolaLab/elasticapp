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

#include <elastica/ErrorHandling/ExpectsAndEnsures.hpp>
#include <elastica/Simulator/Connections.hpp>
#include <elastica/Systems/CosseratRods.hpp>
#include <elastica/Utilities/DefineTypes.h>
#include <elastica/Utilities/Get.hpp>
#include <elastica/Utilities/Math.hpp>
#include <elastica/Utilities/Math/Dot.hpp>
#include <elastica/Utilities/Math/Normalize.hpp>
#include <iostream>
#include <tuple>

namespace econ = ::elastica::connections;

struct ConnectionInfo {
  elastica::Vec3 rod_one_direction_vec_in_material_frame;
  elastica::Vec3 rod_two_direction_vec_in_material_frame;
  elastica::real_t offset_btw_rods;
};

template <typename Traits, template <typename> class ComputationalBlock,
          template <typename /*CRT*/, typename /*InitializedBlock*/>
          class... Components>
void parallel_connection_kernel(
    elastica::cosserat_rod::CosseratRodPlugin<Traits, ComputationalBlock,
                                              Components...>& first_rod_like,
    elastica::cosserat_rod::CosseratRodPlugin<Traits, ComputationalBlock,
                                              Components...>& second_rod_like,
    std::size_t index_begin, std::size_t index_end, elastica::real_t k,
    elastica::real_t nu, elastica::real_t k_repulsive,
    blaze::DynamicVector<elastica::real_t> const&
        rest_offset_btw_rods_collection,
    blaze::DynamicMatrix<elastica::real_t> const&
        rod_one_direction_vec_in_material_frame_collection,
    blaze::DynamicMatrix<elastica::real_t> const&
        rod_two_direction_vec_in_material_frame_collection) {
  auto& first_rod(first_rod_like.self());
  auto& second_rod(second_rod_like.self());

  using ::elastica::real_t;
  using ::elastica::Vec3;

  // hacky but works
  auto const& ghost_nodes = first_rod.parent().get_ghost_node_buffer();
  // Whats the beginning index of the slice
  std::size_t ghost_node_index = first_rod.region().start;

  for (auto loop_index = index_begin; loop_index < index_end; ++loop_index) {
    //      const auto first_index = arg_first_index + 1UL;
    //      const auto second_index = arg_second_index;
    if ((loop_index + 1) == ghost_nodes[ghost_node_index]) {
      continue;
    }
    if (loop_index == ghost_nodes[ghost_node_index]) {
      ++ghost_node_index;
      continue;
    }
    auto const arg_first_index = loop_index;
    auto const arg_second_index = loop_index;
    Vec3 const rod_one_direction_vec_in_material_frame(blaze::column(
        rod_one_direction_vec_in_material_frame_collection, loop_index));
    Vec3 const rod_two_direction_vec_in_material_frame(blaze::column(
        rod_two_direction_vec_in_material_frame_collection, loop_index));
    real_t const rest_offset_btw_rods(
        rest_offset_btw_rods_collection[loop_index]);

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
    real_t rod_one_dilatation =
        (::elastica::get<::elastica::tags::ElementDilatation>(
            first_rod)[arg_first_index]);
    real_t rod_two_dilatation =
        (::elastica::get<::elastica::tags::ElementDilatation>(
            first_rod)[arg_second_index]);
    real_t offset_rod_one =
        0.5 * rest_offset_btw_rods / std::sqrt(rod_one_dilatation);
    real_t offset_rod_two =
        0.5 * rest_offset_btw_rods / std::sqrt(rod_two_dilatation);

    // Compute r*d2
    Vec3 rod_one_rd2 =
        rod_one_to_rod_two_connection_vec *
        (first_rod.get_element_dimension(arg_first_index) + offset_rod_one);
    Vec3 rod_two_rd2 =
        rod_two_to_rod_one_connection_vec *
        (second_rod.get_element_dimension(arg_second_index) + offset_rod_two);

    // Compute connection points on the rod surfaces
    Vec3 surface_position_rod_one = rod_one_element_position + rod_one_rd2;
    Vec3 surface_position_rod_two = rod_two_element_position + rod_two_rd2;

    // Compute spring force between two rods
    Vec3 distance_vector = surface_position_rod_two - surface_position_rod_one;
    Vec3 spring_force = k * (distance_vector);

    // Damping Force
    Vec3 rod_one_element_velocity =
        0.5 * (::elastica::velocity(first_rod, arg_first_index) +
               ::elastica::velocity(first_rod, arg_first_index + 1));
    Vec3 rod_two_element_velocity =
        0.5 * (::elastica::velocity(second_rod, arg_second_index) +
               ::elastica::velocity(second_rod, arg_second_index + 1));
    Vec3 relative_velocity =
        rod_two_element_velocity - rod_one_element_velocity;
    Vec3 damping_force = nu * relative_velocity;

    // Penetration Force
    Vec3 center_distance = rod_two_element_position - rod_one_element_position;
    real_t center_distance_norm = elastica::length(center_distance);
    Vec3 center_distance_unit_vec = center_distance / center_distance_norm;
    real_t penetration_strain =
        center_distance_norm -
        (first_rod.get_element_dimension(arg_first_index) + offset_rod_one +
         second_rod.get_element_dimension(arg_second_index) + offset_rod_two);

    bool is_penetrated = (penetration_strain < 0.0);
    Vec3 contact_force = is_penetrated
                             ? -k_repulsive *
                                   std::pow(std::abs(penetration_strain), 1.5) *
                                   center_distance_unit_vec
                             : elastica::Vec3{0.0, 0.0, 0.0};

    // Compute the total force
    Vec3 half_total_force =
        0.5 * (spring_force + contact_force + damping_force);

    // Add total forces on external load
    elastica::external_loads(first_rod, arg_first_index) += half_total_force;
    elastica::external_loads(first_rod, arg_first_index + 1) +=
        half_total_force;
    elastica::external_loads(second_rod, arg_second_index) -= half_total_force;
    elastica::external_loads(second_rod, arg_second_index + 1) -=
        half_total_force;

    /* Make Torque */
    // Compute torques due to the connection forces

    Vec3 torque_on_rod_one = rod_one_rd2 % spring_force;
    Vec3 torque_on_rod_two = rod_two_rd2 % (-spring_force);

    // this is not a nodal quantitiy
    elastica::external_torques(first_rod, arg_first_index) +=
        (::elastica::director(first_rod, arg_first_index) * torque_on_rod_one);
    elastica::external_torques(second_rod, arg_second_index) +=
        (::elastica::director(second_rod, arg_second_index) *
         torque_on_rod_two);
  }
}

struct ParallelConnection {  // Parallel Connection Impl
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
    Expects(rod_one_end_idx - rod_one_start_idx ==
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
      std::size_t index_begin, std::size_t index_end, const real_t k,
      const real_t nu, const real_t k_repulsive,
      blaze::DynamicMatrix<elastica::real_t>
          rod_one_direction_vec_in_material_frame,
      blaze::DynamicMatrix<elastica::real_t>
          rod_two_direction_vec_in_material_frame,
      blaze::DynamicVector<elastica::real_t> offset_btw_rods) {
    auto element_wise_boundary =
        [=,
         rod_one_direction_vec_in_material_frame_collection =
             std::move(rod_one_direction_vec_in_material_frame),
         rod_two_direction_vec_in_material_frame_collection =
             std::move(rod_two_direction_vec_in_material_frame),
         offset_btw_rods_collection = std::move(offset_btw_rods)](
            auto /*time*/, auto& first_rod, auto& second_rod) {
          parallel_connection_kernel(
              first_rod, second_rod, index_begin, index_end, k, nu, k_repulsive,
              offset_btw_rods_collection,
              rod_one_direction_vec_in_material_frame_collection,
              rod_two_direction_vec_in_material_frame_collection);
        };

    return econ::make_connection(element_wise_boundary);
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
      real_t rod_one_dilatation =
          (::elastica::get<::elastica::tags::ElementDilatation>(
              first_rod)[arg_first_index]);
      real_t rod_two_dilatation =
          (::elastica::get<::elastica::tags::ElementDilatation>(
              first_rod)[arg_second_index]);
      real_t offset_rod_one =
          0.5 * rest_offset_btw_rods / std::sqrt(rod_one_dilatation);
      real_t offset_rod_two =
          0.5 * rest_offset_btw_rods / std::sqrt(rod_two_dilatation);

      // Compute r*d2
      Vec3 rod_one_rd2 =
          rod_one_to_rod_two_connection_vec *
          (first_rod.get_element_dimension(arg_first_index) + offset_rod_one);
      Vec3 rod_two_rd2 =
          rod_two_to_rod_one_connection_vec *
          (second_rod.get_element_dimension(arg_second_index) + offset_rod_two);

      // Compute connection points on the rod surfaces
      Vec3 surface_position_rod_one = rod_one_element_position + rod_one_rd2;
      Vec3 surface_position_rod_two = rod_two_element_position + rod_two_rd2;

      // Compute spring force between two rods
      Vec3 distance_vector =
          surface_position_rod_two - surface_position_rod_one;
      Vec3 spring_force = k * (distance_vector);

      // Damping Force
      Vec3 rod_one_element_velocity =
          0.5 * (::elastica::velocity(first_rod, arg_first_index) +
                 ::elastica::velocity(first_rod, arg_first_index + 1));
      Vec3 rod_two_element_velocity =
          0.5 * (::elastica::velocity(second_rod, arg_second_index) +
                 ::elastica::velocity(second_rod, arg_second_index + 1));
      Vec3 relative_velocity =
          rod_two_element_velocity - rod_one_element_velocity;
      Vec3 damping_force = nu * relative_velocity;

      // Penetration Force
      Vec3 center_distance =
          rod_two_element_position - rod_one_element_position;
      real_t center_distance_norm = elastica::length(center_distance);
      Vec3 center_distance_unit_vec = center_distance / center_distance_norm;
      real_t penetration_strain =
          center_distance_norm -
          (first_rod.get_element_dimension(arg_first_index) + offset_rod_one +
           second_rod.get_element_dimension(arg_second_index) + offset_rod_two);

      bool is_penetrated = (penetration_strain < 0.0);
      Vec3 contact_force =
          is_penetrated
              ? -k_repulsive * std::pow(std::abs(penetration_strain), 1.5) *
                    center_distance_unit_vec
              : elastica::Vec3{0.0, 0.0, 0.0};

      // Compute the total force
      Vec3 half_total_force =
          0.5 * (spring_force + contact_force + damping_force);

      // Add total forces on external load
      elastica::external_loads(first_rod, arg_first_index) += half_total_force;
      elastica::external_loads(first_rod, arg_first_index + 1) +=
          half_total_force;
      elastica::external_loads(second_rod, arg_second_index) -=
          half_total_force;
      elastica::external_loads(second_rod, arg_second_index + 1) -=
          half_total_force;

      /* Make Torque */
      // Compute torques due to the connection forces

      Vec3 torque_on_rod_one = rod_one_rd2 % spring_force;
      Vec3 torque_on_rod_two = rod_two_rd2 % (-spring_force);

      // this is not a nodal quantitiy
      elastica::external_torques(first_rod, arg_first_index) +=
          (::elastica::director(first_rod, arg_first_index) *
           torque_on_rod_one);
      elastica::external_torques(second_rod, arg_second_index) +=
          (::elastica::director(second_rod, arg_second_index) *
           torque_on_rod_two);
    };

    /* spanwise surface side-by-side connection */
    auto spanwise_boundary = [=](auto /*time*/, auto& first_rod,
                                 auto& second_rod) {};

    return econ::make_connection(element_wise_boundary, spanwise_boundary);
  };
};

struct OrthogonalConnection {  // Orthogonal Connection Impl
  using real_t = ::elastica::real_t;
  using Vec3 = ::elastica::Vec3;
  using Traits = ::elastica::cosserat_rod::DefaultCosseratRodTraits;

  using Scalar = Traits::DataType::Scalar;
  using Vector = Traits::DataType::Vector;
  using Matrix = Traits::DataType::Matrix;
  using TOp = Traits::Operations;

  template <typename RodSliceTypeA, typename RodSliceTypeB>
  static auto get_connection_vector_for_perpendicular_rods(
      RodSliceTypeA const& rod_one, RodSliceTypeB const& rod_two,
      const std::size_t rod_one_start_idx,
      const std::size_t rod_two_start_idx) {
    /* get Position vector */
    Vec3 rod_one_element_position =
        0.5 * (::elastica::position(rod_one, rod_one_start_idx) +
               ::elastica::position(rod_one, rod_one_start_idx + 1UL));
    Vec3 rod_two_element_position =
        0.5 * (::elastica::position(rod_two, rod_two_start_idx) +
               ::elastica::position(rod_two, rod_two_start_idx + 1UL));

    /* get Director vector */
    elastica::Rot3 rod_one_director =
        ::elastica::director(rod_one, rod_one_start_idx);
    elastica::Rot3 rod_two_director =
        ::elastica::director(rod_two, rod_two_start_idx);

    /* get Radius vector */
    const real_t rod_one_radius =
        rod_one.get_element_dimension(rod_one_start_idx);
    const real_t rod_two_half_length =
        0.5 * rod_two.get_element_length(rod_two_start_idx);

    /* get rod-to-rod vector */
    Vec3 position_diff = rod_two_element_position - rod_one_element_position;
    real_t distance_vector_rod_one_to_rod_two_norm =
        elastica::length(position_diff);
    Vec3 distance_vector_rod_one_to_rod_two =
        position_diff / distance_vector_rod_one_to_rod_two_norm;
    Vec3 distance_vector_rod_two_to_rod_one(
        -distance_vector_rod_one_to_rod_two);

    /* get direction vector in material frame */
    Vec3 rod_one_direction_vec_in_material_frame =
        rod_one_director * distance_vector_rod_one_to_rod_two;
    Vec3 rod_two_direction_vec_in_material_frame =
        rod_two_director * distance_vector_rod_two_to_rod_one;
    real_t offset_btw_rods = distance_vector_rod_one_to_rod_two_norm -
                             (rod_one_radius + rod_two_half_length);

    return ConnectionInfo{rod_one_direction_vec_in_material_frame,
                          rod_two_direction_vec_in_material_frame,
                          offset_btw_rods};
  }

  static auto perpendicular_connection_generator(
      const real_t k, const real_t nu, const real_t k_repulsive,
      const real_t kt, ConnectionInfo const& connection_info) {
    /* element-wise perpendicular connection */
    auto element_wise_boundary =
        [k = k, nu = nu, k_repulsive = k_repulsive, kt = kt,
         rest_offset_btw_rods = connection_info.offset_btw_rods,
         rod_one_direction_vec_in_material_frame =
             connection_info.rod_one_direction_vec_in_material_frame,
         rod_two_direction_vec_in_material_frame =
             connection_info.rod_two_direction_vec_in_material_frame](
            auto /*time*/, auto& first_rod, std::size_t arg_first_index,
            auto& second_rod, std::size_t arg_second_index) {
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
          real_t rod_one_dilatation =
              (::elastica::get<::elastica::tags::ElementDilatation>(
                  first_rod)[arg_first_index]);
          real_t rod_two_dilatation =
              (::elastica::get<::elastica::tags::ElementDilatation>(
                  second_rod)[arg_second_index]);
          real_t offset_rod_one =
              0.5 * rest_offset_btw_rods / std::sqrt(rod_one_dilatation);
          real_t offset_rod_two =
              0.5 * rest_offset_btw_rods * rod_two_dilatation;

          // Compute r*d2 and l*d2
          Vec3 rod_one_rd2 = rod_one_to_rod_two_connection_vec *
                             (first_rod.get_element_dimension(arg_first_index) +
                              offset_rod_one);
          Vec3 rod_two_ld3 =
              rod_two_to_rod_one_connection_vec *
              (0.5 * second_rod.get_element_length(arg_second_index) +
               offset_rod_two);

          // Compute connection points on the rod surfaces
          Vec3 surface_position_rod_one =
              rod_one_element_position + rod_one_rd2;
          Vec3 surface_position_rod_two =
              rod_two_element_position + rod_two_ld3;

          // Compute spring force between two rods
          Vec3 distance_vector(surface_position_rod_two -
                               surface_position_rod_one);
          //      auto round_up = [](double value, int decimal_places) {
          //        const double multiplier = std::pow(10.0, decimal_places);
          //        return std::ceil(value * multiplier) / multiplier;
          //      };
          //      Vec3 distance_vector{round_up(unrounded_distance_vector[0],
          //      12),
          //                           round_up(unrounded_distance_vector[1],
          //                           12),
          //                           round_up(unrounded_distance_vector[2],
          //                           12)};
          Vec3 spring_force = k * (distance_vector);

          // Damping Force
          Vec3 rod_one_element_velocity =
              0.5 * (::elastica::velocity(first_rod, arg_first_index) +
                     ::elastica::velocity(first_rod, arg_first_index + 1));
          Vec3 rod_two_element_velocity =
              0.5 * (::elastica::velocity(second_rod, arg_second_index) +
                     ::elastica::velocity(second_rod, arg_second_index + 1));
          Vec3 relative_velocity =
              rod_two_element_velocity - rod_one_element_velocity;
          real_t distance = elastica::length(distance_vector);
          Vec3 normalized_distance_vector = distance >= 1e-12
                                                ? distance_vector / distance
                                                : Vec3{0.0, 0.0, 0.0};

          Vec3 normal_relative_velocity_vector =
              dot(relative_velocity, normalized_distance_vector) *
              normalized_distance_vector;
          Vec3 damping_force = nu * normal_relative_velocity_vector;

          // Penetration Force
          Vec3 center_distance =
              rod_two_element_position - rod_one_element_position;
          real_t center_distance_norm = elastica::length(center_distance);
          // Is never 0, so / norm is fine
          Vec3 center_distance_unit_vec =
              center_distance / center_distance_norm;
          real_t penetration_strain =
              center_distance_norm -
              (first_rod.get_element_dimension(arg_first_index) +
               offset_rod_one +
               0.5 * second_rod.get_element_length(arg_second_index) +
               offset_rod_two);
          bool is_penetrated = (penetration_strain < 0.0);
          Vec3 contact_force =
              is_penetrated
                  ? -k_repulsive * std::pow(std::abs(penetration_strain), 1.5) *
                        center_distance_unit_vec
                  : elastica::Vec3{0.0, 0.0, 0.0};

          // Compute the total force
          Vec3 half_total_force =
              0.5 * (spring_force + contact_force + damping_force);

          // Add total forces on external load
          elastica::external_loads(first_rod, arg_first_index) +=
              half_total_force;
          elastica::external_loads(first_rod, arg_first_index + 1) +=
              half_total_force;

          elastica::external_loads(second_rod, arg_second_index) -=
              half_total_force;
          elastica::external_loads(second_rod, arg_second_index + 1) -=
              half_total_force;

          /* Make Torque */
          // Compute torques due to the connection forces

          Vec3 torque_on_rod_one(blaze::cross(rod_one_rd2, spring_force));
          Vec3 torque_on_rod_two(blaze::cross(rod_two_ld3, -spring_force));

          // Moment Arm
          const real_t mag(elastica::length(rod_two_ld3));
          Vec3 moment_arm_dir =
              mag >= 1e-12 ? rod_two_ld3 / mag : Vec3{0.0, 0.0, 0.0};

          Vec3 moment_arm = first_rod.get_element_dimension(arg_first_index) *
                                moment_arm_dir +
                            rod_two_ld3;
          Vec3 rod_one_target_element_position =
              rod_two_element_position + moment_arm;

          distance_vector =
              rod_one_element_position - rod_one_target_element_position;

          Vec3 torque_spring_force = kt * (distance_vector);
          Vec3 spring_torque(blaze::cross(moment_arm, torque_spring_force));

          // This causes the most deviation in forces.
          torque_on_rod_one -= spring_torque;
          torque_on_rod_two += spring_torque;

          // this is not a nodal quantitiy
          ::elastica::external_torques(first_rod, arg_first_index) +=
              (::elastica::director(first_rod, arg_first_index) *
               torque_on_rod_one);
          ::elastica::external_torques(second_rod, arg_second_index) +=
              (::elastica::director(second_rod, arg_second_index) *
               torque_on_rod_two);
        };

    /* spanwise surface side-by-side connection */
    auto spanwise_boundary = [=](auto /*time*/, auto& first_rod,
                                 auto& second_rod) {};

    return econ::make_connection(element_wise_boundary, spanwise_boundary);
  }
};
