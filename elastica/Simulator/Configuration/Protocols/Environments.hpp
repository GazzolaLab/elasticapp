//==============================================================================
/*!
//  \file
//  \brief Environment configuration protocols
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

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Configuration/TypeTraits/Aliases/Environments.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/IsA.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"
#include "Utilities/IgnoreUnused.hpp"

namespace elastica {

  namespace configuration {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Environments configuration protocol.
       * \ingroup configuration_protocols
       *
       * Class to enforce adherence to Environments configuration protocol. Any
       * valid environment configuration within the \elastica library should
       * (publicly) inherit from this class to indicate it qualifies as a
       * Environment. Only in case a class is derived publicly from this base
       * class, the tt::conforms_to and tt::assert_conforms_to type traits
       * recognizes the class as valid Environments.
       *
       * Requires that a conforming type `ConformingType` has these nested types
       * \snippet this expected_types
       *
       * \example
       * ::elastica::configuration::EnvironmentsConfiguration shows a minimal
       * example of conformance to this protocol
       */
      struct EnvironmentsConfiguration {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup configuration_protocols
        */
        template <typename ConformingType>
        struct test {
         public:

          // TODO : whats the protocol here?
          template <typename EnvironmentTag>
          struct ReportTagsProtocolConformation : std::true_type {
//            static_assert(::tt::assert_conforms_to<EnvironmentTag,
//                                                   ::blocks::protocols::Plugin>,
//                          R"error(
//`EnvironmentTag` above does not conform to the protocols expected by a Plugin!
//)error");
          };
          // and all of its contents should conform to a block variable protocol
          template <typename ImposedPrimitive>
          struct ReportImposedPrimitivesProtocolConformation : std::true_type {
//            static_assert(::tt::assert_conforms_to<ImposedPrimitive,
//                                                   ::blocks::protocols::Plugin>,
//                          R"error(
//`ImposedPrimitive` above does not conform to the protocol expected by a Plugin!
//)error");
          };

          /// [expected_types]
          //**Type definitions**************************************************
          // Check for nested types of the ConfirmingType
          static_assert(
              ::tt::is_detected_v<::elastica::tt::admissible_environment_tags_t,
                                  ConformingType>,
              R"error(
Not a conforming Environments configuration, doesn't have a nested type
`AdmissibleEnvironmentTags`
)error");
          using AdmissibleEnvironmentTags =
              ::elastica::tt::admissible_environment_tags_t<ConformingType>;

          static_assert(
              ::tt::is_a_v<tmpl::list, AdmissibleEnvironmentTags>,
              "Admissible environment tags should belong in a typelist!");
          using tags_checks IGNORE_UNUSED = decltype(
              tmpl::transform<AdmissibleEnvironmentTags,
                              ReportTagsProtocolConformation<tmpl::_1>>{});

          static_assert(::tt::is_detected_v<
                            ::elastica::tt::admissible_imposed_primitives_t,
                            ConformingType>,
                        R"error(
Not a conforming Environments configuration, doesn't have a nested type
`AdmissibleImposedPrimitives`
)error");
          using AdmissibleImposedPrimitives =
              ::elastica::tt::admissible_imposed_primitives_t<ConformingType>;
          static_assert(
              ::tt::is_a_v<tmpl::list, AdmissibleImposedPrimitives>,
              "Admissible imposed primitives should belong in a typelist!");
          using primitives_checks IGNORE_UNUSED = decltype(
              tmpl::transform<
                  AdmissibleImposedPrimitives,
                  ReportImposedPrimitivesProtocolConformation<tmpl::_1>>{});

          //! Type of execution policy
          using ExecutionPolicy =
              typename ConformingType::EnvironmentsExecutionPolicy;
          // one of sequential or primitive
          static_assert(
              tmpl::found<tmpl::list<sequential_policy, parallel_policy>,
                          std::is_same<tmpl::_1, ExecutionPolicy>>::value,
              "EnvironmentsExecutionPolicy should either be sequential or "
              "parallel");
          //********************************************************************
          /// [expected_types]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace configuration

}  // namespace elastica
