//==============================================================================
/*!
//  \file
//  \brief Definitions of functions for interfacing blaze types with \elastica
//  states
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Systems/States/Expressions/backends/Declarations.hpp"
///
#include <blaze/math/DynamicMatrix.h>
#include <blaze_tensor/math/DynamicTensor.h>
// this comes from the simulator module, which seems like an anti-pattern
// should be in Systems instead
#include "Simulator/Frames.hpp"

#include "Utilities/Math/BlazeDetail/BlazeRotation.hpp"

namespace elastica {

  namespace states {

    template <typename T, typename RHSVectorBatch>
    auto SO3_primitive_assign(blaze::DynamicTensor<T>& lhs_matrix_batch,
                              blaze::DynamicTensor<T> const& rhs_matrix_batch,
                              RHSVectorBatch const& vector_batch) noexcept // vector bacth includes (omega*dt). Must include minus sign here
        -> void {
//      using ::elastica::exp_batch;
//      exp_batch(lhs_matrix_batch, -vector_batch);
//      lhs_matrix_batch = lhs_matrix_batch * rhs_matrix_batch;
      // TODO: Double check. Place the function externally. Double check the sign
      auto theta = blaze::sqrt(blaze::sum<blaze::columnwise>(vector_batch % vector_batch));
      auto alpha = 1.0 - blaze::cos(theta);
      auto beta = blaze::sin(theta);
      auto u0 = blaze::row(vector_batch, 0UL) / (theta+1e-12);
      auto u1 = blaze::row(vector_batch, 1UL) / (theta+1e-12);
      auto u2 = blaze::row(vector_batch, 2UL) / (theta+1e-12);
      auto q0 = blaze::row(blaze::pageslice(rhs_matrix_batch, 0UL), 0UL);
      auto q1 = blaze::row(blaze::pageslice(rhs_matrix_batch, 0UL), 1UL);
      auto q2 = blaze::row(blaze::pageslice(rhs_matrix_batch, 0UL), 2UL);
      auto q3 = blaze::row(blaze::pageslice(rhs_matrix_batch, 1UL), 0UL);
      auto q4 = blaze::row(blaze::pageslice(rhs_matrix_batch, 1UL), 1UL);
      auto q5 = blaze::row(blaze::pageslice(rhs_matrix_batch, 1UL), 2UL);
      auto q6 = blaze::row(blaze::pageslice(rhs_matrix_batch, 2UL), 0UL);
      auto q7 = blaze::row(blaze::pageslice(rhs_matrix_batch, 2UL), 1UL);
      auto q8 = blaze::row(blaze::pageslice(rhs_matrix_batch, 2UL), 2UL);
      // TODO: maybe replace blaze::pow(x,2) to x % x
      blaze::row(blaze::pageslice(lhs_matrix_batch, 0UL), 0UL) = alpha*((-blaze::pow(u1, 2) - blaze::pow(u2, 2))*q0 + q3*u0*u1 + q6*u0*u2) + beta*( q3*u2 - q6*u1) + q0;
      blaze::row(blaze::pageslice(lhs_matrix_batch, 0UL), 1UL) = alpha*((-blaze::pow(u1, 2) - blaze::pow(u2, 2))*q1 + q4*u0*u1 + q7*u0*u2) + beta*( q4*u2 - q7*u1) + q1;
      blaze::row(blaze::pageslice(lhs_matrix_batch, 0UL), 2UL) = alpha*((-blaze::pow(u1, 2) - blaze::pow(u2, 2))*q2 + q5*u0*u1 + q8*u0*u2) + beta*( q5*u2 - q8*u1) + q2;
      blaze::row(blaze::pageslice(lhs_matrix_batch, 1UL), 0UL) = alpha*((-blaze::pow(u0, 2) - blaze::pow(u2, 2))*q3 + q0*u0*u1 + q6*u1*u2) + beta*(-q0*u2 + q6*u0) + q3;
      blaze::row(blaze::pageslice(lhs_matrix_batch, 1UL), 1UL) = alpha*((-blaze::pow(u0, 2) - blaze::pow(u2, 2))*q4 + q1*u0*u1 + q7*u1*u2) + beta*(-q1*u2 + q7*u0) + q4;
      blaze::row(blaze::pageslice(lhs_matrix_batch, 1UL), 2UL) = alpha*((-blaze::pow(u0, 2) - blaze::pow(u2, 2))*q5 + q2*u0*u1 + q8*u1*u2) + beta*(-q2*u2 + q8*u0) + q5;
      blaze::row(blaze::pageslice(lhs_matrix_batch, 2UL), 0UL) = alpha*((-blaze::pow(u0, 2) - blaze::pow(u1, 2))*q6 + q0*u0*u2 + q3*u1*u2) + beta*( q0*u1 - q3*u0) + q6;
      blaze::row(blaze::pageslice(lhs_matrix_batch, 2UL), 1UL) = alpha*((-blaze::pow(u0, 2) - blaze::pow(u1, 2))*q7 + q1*u0*u2 + q4*u1*u2) + beta*( q1*u1 - q4*u0) + q7;
      blaze::row(blaze::pageslice(lhs_matrix_batch, 2UL), 2UL) = alpha*((-blaze::pow(u0, 2) - blaze::pow(u1, 2))*q8 + q2*u0*u2 + q5*u1*u2) + beta*( q2*u1 - q5*u0) + q8;
    }

    template <typename T, typename RHSVectorBatch>
    auto SO3_primitive_add_assign(blaze::DynamicTensor<T>& lhs_matrix_batch,
                                  RHSVectorBatch const& rhs_vector) noexcept
        -> void {
      auto temp_to_synch(lhs_matrix_batch); // TODO: avoid making new memory (but not sure how)
      // The rotation computation cannot be computed asynchronously during add_assign.
      SO3_primitive_assign(lhs_matrix_batch, temp_to_synch, rhs_vector);
    }

    template <typename T>
    inline auto size_backend(blaze::DynamicTensor<T> const& tensor) noexcept
        -> std::size_t {
      return tensor.columns();
    }

    template <typename Type,   // Data type of the matrix
              bool SO,         // Storage order
              typename Alloc,  // Type of the allocator
              typename Tag>    // Type tag
    inline auto size_backend(
        blaze::DynamicMatrix<Type, SO, Alloc, Tag> const& tensor) noexcept
        -> std::size_t {
      return tensor.columns();
    }

    // the parts below are repeated from the Block module, but they are almost
    // always never used in the states module since the size is always expected
    // to be
    template <typename T>
    inline auto resize_backend(blaze::DynamicTensor<T>& data,
                               std::size_t new_size) -> void {
      // usually is not executed
      return data.resize(Frames::Dimension, Frames::Dimension, new_size, true);
    }

    template <typename Type,   // Data type of the matrix
              bool SO,         // Storage order
              typename Alloc,  // Type of the allocator
              typename Tag>    // Type tag
    inline auto resize_backend(blaze::DynamicMatrix<Type, SO, Alloc, Tag>& data,
                               std::size_t new_size) -> void {
      // usually is not executed
      return data.resize(Frames::Dimension, new_size, true);
    }

  }  // namespace states

}  // namespace elastica
