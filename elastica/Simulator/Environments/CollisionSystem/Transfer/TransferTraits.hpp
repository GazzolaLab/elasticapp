//==============================================================================
/*!
//  \file
//  \brief Transfer traits
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

//// Declare all types
#include "Simulator/Environments/CollisionSystem/Transfer/Types.hpp"
////
#include "Simulator/Environments/CollisionSystem/Transfer/TransferTags.hpp"
#include "Simulator/Interface/TypeTraits/Aliases/Systems.hpp"

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
    template <typename SystemTag>
    struct TransferTraits {
      //**Type definitions******************************************************
      using from_t = tags::from_t;  //!< From type of transfer
      using to_t = tags::to_t;      //!< To type of transfer

      //! Type of system
      using SystemType = tt::system_type_t<SystemTag>;
      //! Type of contained primitive
      using PrimitiveType = tt::primitive_type_t<SystemTag>;
      //************************************************************************

      /* NOT POSSIBLE
       * Here we want to declare the types of PrimitiveIteratorType and
       * ConstPrimitiveIteratorType apriori like so
       *
       * using PrimitiveIteratorType =
       * std::iterator_traits<AddPrimtiveStorage<typename
       * BodyTag::PrimitiveType>::iterator>::value_type;
       *
       * , but this cannot be achieved
       * unless we know the type of the system storage (serial/parallel).
       */
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
