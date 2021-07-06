//==============================================================================
/*!
//  \file
//  \brief Forcing protocols
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

#include <utility>

#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace forcing {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Forcing protocol.
       * \ingroup protocols forcing
       *
       * Class to enforce adherence to Forcing protocol. Any valid forcing
       * within the \elastica library should (publicly) inherit from this class
       * to indicate it qualifies as a Constraint. Only in case a class is
       * derived publicly from this base class, the tt::conforms_to and
       * tt::assert_conforms_to type traits recognizes the class as a valid
       * forcing.
       *
       * Requires that conforming class has these nested types:
       * - \a bound_type : Type of upper/lower bound
       *
       * Requires that the conforming class has these templated member functions
       * templated on the directions
       * - \c get_lower_bound : Returns the lower bound of the domain
       * - \c get_upper_bound : Returns the upper bound of the domain
       * - \c get_span : Returns the span of the domain
      */
      struct Forcing {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup protocols
        */
        template <typename ConformingType>
        struct test {
         public:
          // TODO
        };
        /*! \endcond */
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace protocols

  }  // namespace forcing

}  // namespace elastica
