//==============================================================================
/*!
//  \file
//  \brief Connection interface protocols
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

#include <string>
#include <type_traits>
#include <utility>  // declval

#include "Utilities/NoSuchType.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace elastica {

  namespace interface {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Connection interface protocol.
       * \ingroup interface_protocols
       *
       * Class to enforce adherence to a Connection interface protocol. Any
       * valid Connection interface class within the \elastica library
       * should (publicly) inherit from this class to indicate it qualifies as a
       * Connection interface. Only in case a class is derived publicly from
       * this base class, the tt::conforms_to and tt::assert_conforms_to type
       * traits recognizes the class as valid connections module.
       *
       * Requires that a conforming type `ConformingType` has these nested types
       * \snippet this expected_types
       * and these member functions,
       * \snippet this expected_functions
       *
       * \example
       * The following shows an example of minimal conformance to this protocol.
       * \snippet Interface/Protocols/Test_Connections.cpp connections_eg
       */
      struct Connections {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup interface_protocols
        */
        template <typename ConformingType>
        struct test {
          /// [expected_types]
          //**Type definitions**************************************************
          //! Time type of the configured module
          using time_type = typename ConformingType::TimeType;
          //********************************************************************
          /// [expected_types]

          /// [expected_functions]
          using return_type =
              decltype(std::declval<ConformingType const&>().apply_connections(
                  std::declval<time_type>()));
          // Confirm that there are no side effects upon calling this function
          // by comparing strictly against void
          static_assert(cpp17::is_same_v<return_type, void>,
                        R"error(
Not a conforming Connection, doesn't properly implement function
`void apply_connections(time_type time_v) const`
)error");

          using FirstSystemType = NoSuchType;
          using SecondSystemType = NoSuchType;
          using Name = std::string;
          using connect_return_type =
              decltype(std::declval<ConformingType&>().connect(
                  std::declval<FirstSystemType const&>(),
                  std::declval<SecondSystemType const&>(),
                  std::declval<Name>()));
          /// [expected_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace interface

}  // namespace elastica