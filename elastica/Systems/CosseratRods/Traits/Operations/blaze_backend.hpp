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

/// Add blaze_backend code here

namespace elastica {

  namespace cosserat_rod {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Rod Operations specialized for blaze data structures
     * \ingroup cosserat_rods
     */
    template <OptimizationLevel O>
    struct BlazeOperations;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for non-optimized routines
     * \ingroup cosserat_rods
     */
    template <>
    struct BlazeOperations<OptimizationLevel::basic> : public NonCreatable {
      static inline int add(int x, int y) { return x + y; }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for optimized routines
     * \ingroup cosserat_rods
     */
    template <>
    struct BlazeOperations<OptimizationLevel::optimized> : public NonCreatable {
      static inline int add(int x, int y) { return x + y; }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica