//==============================================================================
/*!
//  \file
//  \brief Protocols for geometric primitives
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

#include <type_traits>
#include <utility>

#include "Utilities/Math/Vec3.hpp"

namespace elastica {

  namespace collision {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Convex primitive protocol.
       * \ingroup primitives
       *
       * Class to enforce adherence to a Primitive protocol expected by the
       * collision detection library in \elastica. Any valid primitive within
       * the \elastica library should (publicly) inherit from this class to
       * indicate it qualifies as a DEM Force. Only in case a class is derived
       * publicly from this base class, the  tt::conforms_to and
       * tt::assert_conforms_to type traits recognizes the class as valid.
       *
       * Requires that a conforming type `ConformingType` has these nested
       * member functions
       * \snippet this expected_functions
       *
       * \example
       * The following shows an example of minimal conformance to this protocol.
       * \snippet DEMForces/Test_Protocols.cpp protocols_eg
       */
      struct ConvexPrimitive {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup protocols
        */
        template <typename ConformingType>
        struct test {
          /// [expected_functions]
          // Try calling the `ConformingType::support` member function
          using support_return_type =
              decltype(std::declval<ConformingType const&>().support(
                  std::declval<Vec3>()));

          // Try calling the `ConformingType::support_contact_threshold` member
          // function
          using support_contact_threshold_return_type =
              decltype(std::declval<ConformingType const&>()
                           .support_contact_threshold(std::declval<Vec3>()));

          // Check the return type of the `support` member function
          static_assert(std::is_same_v<support_return_type, Vec3>,
                        R"error(
Not a conforming primitive, doesn't properly implement function
`Vec3 support(const Vec3&)`)error");

          // Check the return type of the `support_contact_threshold` member
          // function
          static_assert(
              std::is_same_v<support_contact_threshold_return_type, Vec3>,
              R"error(
Not a conforming primitive, doesn't properly implement function
`Vec3 support_contact_threshold(const Vec3&)`)error");
          /// [expected_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace collision

}  // namespace elastica