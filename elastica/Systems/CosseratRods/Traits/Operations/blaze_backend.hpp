//==============================================================================
/*!
//  \file
//  \brief Operations for blaze backend within Cosserat Rod kernels
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
#include "Systems/CosseratRods/Traits/Operations/OptimizationLevel.hpp"
#include "Utilities/NonCreatable.hpp"
#include "Utilities/Math/Linalg.hpp"

/// Add blaze_backend code here

namespace elastica {

  namespace cosserat_rod {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Rod Operations specialized for blaze data structures
     * \ingroup cosserat_rods
     */
    template <OptimizationLevel O, typename TraitsForData>
    struct BlazeOperations : private NonCreatable{
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for non-optimized routines
     * \ingroup cosserat_rods
     */
    template <typename TraitsForData>
    struct BlazeOperations<OptimizationLevel::basic, TraitsForData> : private NonCreatable {
      using DataTrait = TraitsForData;

      static inline int add(int x, int y) { return x + y; }

      //************************************************************************
      // Batch Operations
      static inline void batch_matvec(typename DataTrait::Vector& matvec_batch,
                                      typename DataTrait::Matrix& matrix_batch,
                                      typename DataTrait::Vector& vector_batch) {
        elastica::batch_matvec<DataTrait::Vector, DataTrait::Matrix, DataTrait::Vector>
            (matvec_batch, matrix_batch, vector_batch);
      }

      static inline void batch_matmul(typename DataTrait::Matrix& matmul_batch,
                                      typename DataTrait::Matrix& first_matrix_batch,
                                      typename DataTrait::Matrix& second_matrix_batch) {
        elastica::batch_matvec<DataTrait::Matrix, DataTrait::Matrix, DataTrait::Matrix>
            (matmul_batch, first_matrix_batch, second_matrix_batch);
      }

      static inline void batch_cross(typename DataTrait::Vector& cross_batch,
                                     typename DataTrait::Vector& first_vector_batch,
                                     typename DataTrait::Vector& second_vector_batch) {
        elastica::batch_cross<DataTrait::Vector, DataTrait::Vector, DataTrait::Vector>
            (cross_batch, first_vector_batch, second_vector_batch);
      }

      static auto batch_dot(typename DataTrait::Vector& first_vector_batch,
                            typename DataTrait::vector& second_vector_batch) {
        return elastica::batch_dot<DataTrait::Vector, DataTrait::Vector> (
            first_vector_batch, second_vector_batch);
      }

      static auto batch_norm(typename DataTrait::Vector& vector_batch) {
        return elastica::batch_norm<DataTrait::Vector> (vector_batch);
      }

      //************************************************************************
      // Vector Operation
      static inline void difference_kernel(typename DataTrait::Vector& result_vector,
                                           typename DataTrait::Vector& input_vector,){
        elastica::difference_kernel<DataTrait::Vector, DataTrait::Vector> (
            result_vector, input_vector
            );
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for optimized routines
     * \ingroup cosserat_rods
     */
    template <typename TraitsForData>
    struct BlazeOperations<OptimizationLevel::optimized, TraitsForData> : private NonCreatable {
      static inline int add(int x, int y) { return x + y; }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica