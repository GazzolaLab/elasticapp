//==============================================================================
/*!
//  \file
//  \brief Environment protocols
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

//
#include "Simulator/Environments/Common/Protocols.hpp"
//
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace environments {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Protocol for environment tags
       * \ingroup environments
       *
       * Class to enforce adherence to the protocol for environment tags. Any
       * valid enviroment tag within the \elastica library should (publicly)
       * inherit from this class to indicate it qualifies as a valid System.
       * Only in case a class is derived publicly from this base class, the
       * tt::conforms_to and tt::assert_conforms_to type traits recognizes the
       * class as valid Environment tag type
       *
       * Requires that a conforming type `ConformingType` has these
       * metavariables
       * \snippet this expected_metavariables
       *
       * The following example shows a compliant class
       * \example
       * \snippet Environments/Test_Protocols.cpp environment_tag_protocol_eg
       */
      struct EnvironmentTag {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        struct Signatures {
          // [expected_metavariables]
          template <typename ConformingType>
          using requires_primitives_result =
              typename ConformingType::requires_primitives;
          // [expected_metavariables]
        };
        /*! \endcond */
        //**********************************************************************

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
         * \ingroup systems
         */
        template <typename ConformingType>
        struct test {
          using has_primitives_result =
              ::tt::is_detected<Signatures::requires_primitives_result,
                                ConformingType>;
          static_assert(has_primitives_result::value,
                        R"error(
Not a conforming EnvironmentTag, doesn't have a member typedef
`using requires_primitives = /* true or false type*/`
)error");
          static_assert(
              std::disjunction_v<
                  std::conditional_t<
                      has_primitives_result::value,
                      // need to protect in case its false
                      tmpl::apply<
                          tmpl::bind<
                              Signatures::template requires_primitives_result,
                              tmpl::_1>,
                          ConformingType>,
                      std::false_type>,
                  std::true_type>,
              R"error(
Not a conforming EnvironmentTag, member typedef requires_primitives is not a
std::true_type or std::false_type
)error");
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace environments

}  // namespace elastica