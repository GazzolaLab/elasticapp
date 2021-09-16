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
    struct BlazeOperations : public NonCreatable{
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for non-optimized routines
     * \ingroup cosserat_rods
     */
    template <typename TraitsForData>
    struct BlazeOperations<OptimizationLevel::basic, TraitsForData> : public NonCreatable {
      using DataTrait = TraitsForData;

      static inline int add(int x, int y) { return x + y; }

      static inline void batch_matvec(typename DataTrait::Vector& matvec_batch,
                                      typename const DataTrait::Matrix& matrix_batch,
                                      typename const DataTrait::Vector& vector_batch) {
        elastica::batch_matvec<DataTrait::Vector,
                               DataTrait::Matrix,
                               DataTrait::vector>
            (matvec_batch, matrix_batch, vector_batch);
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
    struct BlazeOperations<OptimizationLevel::optimized, TraitsForData> : public NonCreatable {
      static inline int add(int x, int y) { return x + y; }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica