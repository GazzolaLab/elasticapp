//==============================================================================
/*!
//  \file
//  \brief Forcing interface protocols
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

namespace elastica {

  namespace interface {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Forcing interface protocol.
       * \ingroup interface_protocols
       *
       * Class to enforce adherence to a Forcing interface protocol. Any
       * valid Forcing interface class within the \elastica library
       * should (publicly) inherit from this class to indicate it qualifies as a
       * Forcing interface. Only in case a class is derived publicly from
       * this base class, the tt::conforms_to and tt::assert_conforms_to type
       * traits recognizes the class as valid forcing module.
       *
       * Requires that a conforming type `ConformingType` has these nested types
       * \snippet this expected_types
       * and these member functions,
       * \snippet this expected_functions
       */
      struct Forcing {
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
          // Try calling the `ConformingType::apply_connections()` member
          // function
          using return_type =
              decltype(std::declval<ConformingType const&>().apply_dynamics(
                  std::declval<time_type>()));
          // Confirm that there are no side effects upon calling this function
          // by comparing strictly against void
          static_assert(std::is_same_v<return_type, void>,
                        R"error(
Not a conforming Forcing, doesn't properly implement function
`void apply_dynamics(time_type time_v) const`
)error");

          using SystemType = NoSuchType;
          using Name = std::string;
          using force_return_type =
              decltype(std::declval<ConformingType&>().force(
                  std::declval<SystemType const&>(), std::declval<Name>()));
          /// [expected_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace interface

}  // namespace elastica
