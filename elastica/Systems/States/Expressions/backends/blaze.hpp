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

namespace elastica {

  namespace states {

    template <typename T, typename RHSVectorBatch>
    auto SO3_primitive_assign(blaze::DynamicTensor<T>& lhs_matrix_batch,
                              blaze::DynamicTensor<T> const& /*rhs_matrix_batch*/,
                              RHSVectorBatch const& /*vector_batch*/) noexcept // vector bacth includes (omega*dt)
        -> void {
      lhs_matrix_batch = 1.0;
    }

    template <typename T, typename RHSVectorBatch>
    auto SO3_primitive_add_assign(blaze::DynamicTensor<T>& lhs_matrix_batch,
                                  RHSVectorBatch const& /*rhs_vector*/) noexcept
        -> void {
      lhs_matrix_batch = 1.0;
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
