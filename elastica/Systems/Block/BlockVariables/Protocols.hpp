//==============================================================================
/*!
//  \file
//  \brief Block protocols
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

#include "Aliases.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace blocks {

  namespace protocols {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Block variable protocol.
    // \ingroup blocks
    //
    // Class to enforce adherence to interface expected by Variables in blocks.
    // Any valid blocks Variable within the \elastica library should (publicly)
    // conform to this class using
    // \code
       tt::ConformsTo<protocols::Variable>
       \endcode
    // to indicate it qualifies as a block Variable. Only in case a class
    // expresses such conformance, the ::tt::conforms_to and
    // ::tt::assert_conforms_to type traits recognizes the class as valid
    // Variable.
    //
    // Requires that conforming type has the following types:
    // \snippet BlockVariables/Aliases.hpp parameter_t
    // \snippet BlockVariables/Aliases.hpp rank_t
    // \snippet BlockVariables/Aliases.hpp slice_type_t
    // \snippet BlockVariables/Aliases.hpp const_slice_type_t
    */
    struct Variable {
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Auxiliary helper struct for enforcing protocols.
      // \ingroup protocols
      */
      template <typename ConformingType>
      struct test {
       public:
        // Check for nested types of the ConfirmingType
        static_assert(::tt::is_detected_v<parameter_t, ConformingType>,
                      "Not a conforming Block Variable, doesn't have a "
                      "nested Parameter type");
        static_assert(::tt::is_detected_v<rank_t, ConformingType>,
                      "Not a conforming Block Variable, doesn't have a "
                      "nested Rank type");
        static_assert(::tt::is_detected_v<slice_type_t, ConformingType>,
                      "Not a conforming Block Variable, doesn't have a "
                      "nested SliceType type");
        static_assert(::tt::is_detected_v<const_slice_type_t, ConformingType>,
                      "Not a conforming Block Variable, doesn't have a "
                      "nested ConstSliceType type");
      };
      /*! \endcond */
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace protocols

}  // namespace blocks