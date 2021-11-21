//==============================================================================
/*!
//  \file
//  \brief System configuration protocols
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

#include "Systems/Protocols.hpp"
///
#include "Simulator/Configuration/ModuleConfiguration/Types.hpp"
#include "Simulator/Configuration/TypeTraits/Aliases/Systems.hpp"
///
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/Cpp20.hpp"
#include "Utilities/TypeTraits/IsConvertibleToA.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace configuration {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Systems configuration protocol.
       * \ingroup configuration_protocols
       *
       * Class to enforce adherence to Systems configuration protocol. Any valid
       * system configuration within the \elastica library should
       * (publicly) inherit from this class to indicate it qualifies as a
       * System Configuration. Only in case a class is derived publicly from
       * this base class, the tt::conforms_to and tt::assert_conforms_to
       * type traits recognizes the class as valid System Configuration.
       *
       * Requires that a conforming type `ConformingType` has these nested types
       * \snippet this expected_types
       * and these member functions
       * \snippet this expected_functions
       *
       * \example
       * ::elastica::configuration::SystemsConfiguration shows a minimal
       * example of conformance to this protocol
       */
      struct SystemsConfiguration {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup configuration_protocols
        */
        template <typename ConformingType>
        struct test {
         public:
          // and all of its contents should conform to a block variable protocol
          template <typename SystemPlugin>
          struct ReportPluginsProtocolConformation : std::true_type {
            static_assert(::tt::assert_conforms_to<SystemPlugin,
                                                   ::blocks::protocols::Plugin>,
                          R"error(
`SystemPlugin` above does not conform to the protocols expected by a Plugin!
)error");
//            static_assert(
//                ::tt::assert_conforms_to<SystemPlugin,
//                                         ::elastica::protocols::PhysicalSystem>,
//                R"error(
//`SystemPlugin` above does not conform to the protocols expected by a PhysicalSystem!
//)error");
          };

          /// [expected_types]
          static_assert(
              ::tt::is_detected_v<::elastica::tt::admissible_system_plugins_t,
                                  ConformingType>,
              R"error(
Not a conforming Systems configuration, doesn't have a nested type
`AdmissibleSystemPlugins`
)error");
          using AdmissibleSystemPlugins =
              ::elastica::tt::admissible_system_plugins_t<ConformingType>;
          static_assert(
              ::tt::is_a_v<tmpl::list, AdmissibleSystemPlugins>,
              "Admissible system tags should reside in a tmpl::list!");
          using plugin_checks IGNORE_UNUSED = decltype(
              tmpl::transform<AdmissibleSystemPlugins,
                              ReportPluginsProtocolConformation<tmpl::_1>>{});

          static_assert(::tt::is_detected_v<::elastica::tt::blocking_policy_t,
                                            ConformingType>,
                        R"error(
Not a conforming Systems configuration, doesn't have a nested type
`BlockingPolicy`
)error");
          using blocking_policy =
              ::elastica::tt::blocking_policy_t<ConformingType>;
          // check conformation of blocking policy
          static_assert(::tt::is_convertible_to_a<
                            ::elastica::configuration::BlockingPolicy,
                            blocking_policy>::value,
                        R"error(
The blocking policy found is not valid. Please ensure that its instantiated by
::elastica::configuration::make_blocking_policy().
)error");

          static_assert(::tt::is_detected_v<::elastica::tt::iteration_policy_t,
                                            ConformingType>,
                        R"error(
Not a conforming Systems configuration, doesn't have a nested type
`IterationPolicy`
)error");
          using iteration_policy =
              ::elastica::tt::iteration_policy_t<ConformingType>;
          static_assert(::tt::is_convertible_to_a<
                            ::elastica::configuration::IterationPolicy,
                            iteration_policy>::value,
                        R"error(
The blocking policy found is not valid. Please ensure that its instantiated by
::elastica::configuration::make_iteration_policy().
)error");
          /// [expected_types]

          /// [expected_functions]
          using const_lvalue_b_return_type = cpp20::remove_cvref_t<
              decltype(std::declval<ConformingType const&>()
                           .blocking_policy())>;
          static_assert(
              cpp17::is_same_v<const_lvalue_b_return_type, blocking_policy>,
              R"error(
Not a conforming Systems configuration, doesn't properly implement function
`blocking_policy const& blocking_policy() const&`
)error");

          using rvalue_b_return_type = cpp20::remove_cvref_t<
              decltype(std::declval<ConformingType&&>().blocking_policy())>;
          static_assert(cpp17::is_same_v<rvalue_b_return_type, blocking_policy>,
                        R"error(
Not a conforming Systems configuration, doesn't properly implement function
`blocking_policy && blocking_policy() &&`
)error");

          using const_lvalue_i_return_type = cpp20::remove_cvref_t<
              decltype(std::declval<ConformingType const&>()
                           .iteration_policy())>;
          static_assert(
              cpp17::is_same_v<const_lvalue_i_return_type, iteration_policy>,
              R"error(
Not a conforming Systems configuration, doesn't properly implement function
`iteration_policy const& iteration_policy() const&`
)error");

          using rvalue_i_return_type = cpp20::remove_cvref_t<
              decltype(std::declval<ConformingType&&>().iteration_policy())>;
          static_assert(
              cpp17::is_same_v<rvalue_i_return_type, iteration_policy>,
              R"error(
Not a conforming Systems configuration, doesn't properly implement function
`iteration_policy && iteration_policy() &&`
)error");
          /// [expected_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace configuration

}  // namespace elastica
