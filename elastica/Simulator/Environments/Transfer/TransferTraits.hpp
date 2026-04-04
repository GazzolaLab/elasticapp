//==============================================================================
/*!
//  \file
//  \brief Transfer traits
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

//// Declare all types
#include "Types.hpp"
////
#include "Simulator/Environments/Primitives/TypeTraits.hpp"
#include "TransferTags.hpp"

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Traits for enabling simulator--collision transfer
    // \ingroup transfer
    //
    // The TransferTraits template struct assists customization of the transfer
    // function between a \system contained in an \elastica Simulator with
    // a \primitive of the \elastica collision library. The Transfer class
    // needs to derive from this class to gain access to the concrete types to
    // customize its operations.
    //
    // \see Transfer
    */
    template <typename System>
    struct TransferTraits {
      //**Type definitions******************************************************
      using from_t = tags::from_t;  //!< From type of transfer
      using to_t = tags::to_t;      //!< To type of transfer

      //! Type of system
      using SystemType = System;
      //! Type of contained primitive
      using PrimitiveType = tt::PrimitiveTrait_t<System>;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
