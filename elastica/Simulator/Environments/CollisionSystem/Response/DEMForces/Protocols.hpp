//==============================================================================
/*!
//  \file
//  \brief Protocols for DEM forcing functions
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

#include <type_traits>
#include <utility>
//
#include "Simulator/Environments/CollisionSystem/Contact/Types.hpp"
//
#include "Utilities/DefineTypes.h"

namespace elastica {

  namespace collision {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief DEM Force model protocol.
       * \ingroup collision_response
       *
       * Class to enforce adherence to a Force protocol expected by the DEM
       * response model. Any valid Force within the \elastica library should
       * (publicly) inherit from this class to indicate it qualifies as a
       * DEM Force. Only in case a class is derived publicly from this base
       * class, the  tt::conforms_to and tt::assert_conforms_to type traits
       * recognizes the class as valid.
       *
       * Requires that a conforming type `ConformingType` has these nested
       * member functions
       * \snippet this expected_functions
       *
       * \example
       * The following shows an example of minimal conformance to this protocol.
       */
      struct DEMForce {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup protocols
        */
        template <typename ConformingType>
        struct test {
         public:
          /// [expected_functions]
          // Try calling the `ConformingType::get_span` templated member
          // function
          using op_return_type =
              decltype(std::declval<ConformingType&>().operator()(
                  std::declval<Contact&>()));
          using penetration_depth_type = real_t;
          static_assert(
              std::is_same_v<op_return_type, penetration_depth_type>,
              R"error(
Not a conforming DEMForce, doesn't properly implement function
`real_t operator()(ContactID)`)error");

          static_assert(std::is_default_constructible<ConformingType>::value,
                        "Conforming type is not default constructible!");
          /// [expected_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace collision

}  // namespace elastica