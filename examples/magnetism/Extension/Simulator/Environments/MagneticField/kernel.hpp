//==============================================================================
/*!
//  \file
//  \brief Magnetic field kernel
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

// for now types are hardcoded
#include <blaze/math/DynamicMatrix.h>
#include <blaze_tensor/math/DynamicTensor.h>
#include <elastica/Utilities/Math/Vec3.hpp>

namespace elastica {

  namespace environments {

    namespace detail {

      // For now the types are hardcoded, but can be exported if needed
      template <typename T>
      inline void kernel(blaze::DynamicMatrix<T>& torques,
                         blaze::DynamicTensor<T> const& directors,
                         blaze::DynamicMatrix<T> const& magnetization,
                         ::elastica::Vec3 const& field) noexcept {
        const std::size_t n_dofs = magnetization.columns();
        // 0. allocate memory for rolling window
        T q[9UL];
        /*
         * structure is
         * q0 q3 q6
         * q1 q4 q7
         * q2 q5 q8
         */
        T mag[3UL];
        T temp[3UL];
        // T theta, alpha, beta;  // doesnt matter the extra memory

        for (std::size_t i_dof = 0UL; i_dof < n_dofs; ++i_dof) {
#pragma unroll 3UL  // needs to be timed
          for (std::size_t k_dim = 0UL; k_dim < 3UL; ++k_dim) {
#pragma unroll 3UL
            for (std::size_t j_dim = 0UL; j_dim < 3UL; ++j_dim) {
              // convention according to SO3PrimitiveAssign
              q[3UL * k_dim + j_dim] = directors(k_dim, j_dim, i_dof);
            }
          }

          // matvec : convert field to local coordinates
#pragma unroll 3UL
          for (std::size_t j_dim = 0UL; j_dim < 3UL; ++j_dim) {
            temp[j_dim] = q[3UL * j_dim + 0UL] * field[0UL] +
                          q[3UL * j_dim + 1UL] * field[1UL] +
                          q[3UL * j_dim + 2UL] * field[2UL];
          }

#pragma unroll 3UL
          for (std::size_t j_dim = 0UL; j_dim < 3UL; ++j_dim) {
            // bad access pattern
            mag[j_dim] = magnetization(j_dim, i_dof);
          }

          // cross product
          torques(0UL, i_dof) += (mag[1UL] * temp[2UL] - mag[2UL] * temp[1UL]);
          torques(1UL, i_dof) += (mag[2UL] * temp[0UL] - mag[0UL] * temp[2UL]);
          torques(2UL, i_dof) += (mag[0UL] * temp[1UL] - mag[1UL] * temp[0UL]);
        }
      }

    }  // namespace detail

  }  // namespace environments

}  // namespace elastica