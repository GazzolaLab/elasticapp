//
// Created by Seung Hyun Kim on 9/16/21.
//

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Systems/CosseratRods/Traits/Operations/OptimizationLevel.hpp"
#include "Systems/CosseratRods/Traits/Operations/blaze_backend.hpp"
#include "Utilities/NonCreatable.hpp"

namespace elastica{

  namespace cosserat_rod{

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Traits implementing operations for Cosserat rods in \elastica
     * \ingroup cosserat_rod
     *
     * \details
     *
     * \see elastica::cosserat_rod::CosseratRodTraits
     */
    template <typename TraitsForData>
    struct OperationTrait: private NonCreatable {
      //**Type definitions******************************************************
      using DataTrait = TraitsForData;

      using Opration=BlazeOperations<OptimizationLevel::optimized, DataTrait>;
      //************************************************************************
    };
    //**************************************************************************

  } // namespace cosserat_rod
} // namespace elastica