//==============================================================================
/*!
//  \file
//  \brief Connection protocols
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

  namespace connections {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Connection protocol.
      // \ingroup protocols connections
      //
      // Class to enforce adherence to Connection protocol. Any valid connection
      // within the \elastica library should (publicly) inherit from this class
      // to indicate it qualifies as a Connection. Only in case a class is
      // derived publicly from this base class, the \c tt::conforms_to and
      // \c tt::assert_conforms_to type traits recognizes the class as a valid
      // connection.
      //
      // Requires that conforming class has these nested types:
      // - \a bound_type : Type of upper/lower bound
      //
      // Requires that the conforming class has these templated member
      // functions, templated on the directions
      // - \c get_lower_bound : Returns the lower bound of the domain
      // - \c get_upper_bound : Returns the upper bound of the domain
      // - \c get_span : Returns the span of the domain
      */
      struct Connection {
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

  }  // namespace connections

}  // namespace elastica
