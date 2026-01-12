//==============================================================================
/*!
//  \file
//  \brief Environment protocols
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

#include "Simulator/Environments/Common/Access.hpp"
//
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace environments {

    namespace protocols {

      namespace detail {
        using ::elastica::environments::initialize_parameters;
        using ::elastica::environments::reset;

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        struct Signatures {
          // [expected_metavariables]
          template <typename ConformingType>
          using time_type_result = typename ConformingType::TimeType;
          // [expected_metavariables]

          // [signatures]
          template <typename ConformingType>
          using reset_result =
              decltype(reset(std::declval<ConformingType&>(),
                             std::declval<time_type_result<ConformingType>>()));
          template <typename ConformingType, typename Args>
          using initialize_parameters_result = decltype(initialize_parameters(
              std::declval<ConformingType&>(), std::declval<Args const&>()));

          // [signatures]
        };
        /*! \endcond */
        //**********************************************************************

      }  // namespace detail

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Protocol for environments
       * \ingroup environments
       *
       * Class to enforce adherence to the protocol for environments. Any
       * valid enviroment within the \elastica library should (publicly)
       * inherit from this class to indicate it qualifies as a valid System.
       * Only in case a class is derived publicly from this base class, the
       * tt::conforms_to and tt::assert_conforms_to type traits recognizes the
       * class as valid Environment
       *
       * Requires that a conforming type `ConformingType` has these
       * metavariables
       * \snippet this expected_metavariables
       * and these member functions
       * \snippet this expected_functions
       * and these free functions
       * \snippet this signatures
       *
       *
       * The following example shows a compliant class
       * \example
       * \snippet Environments/Test_Protocols.cpp environment_tag_protocol_eg
       */
      struct Environment {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
         * \ingroup systems
         */
        template <typename ConformingType>
        struct test {
          /// [expected_metavariables]
          static_assert(
              ::tt::is_detected_v<detail::Signatures::time_type_result,
                                  ConformingType>,
              R"error(
Not a conforming Environment, doesn't have typedef
`using TimeType = /*implementation defined*/`
)error");
          /// [expected_metavariables]

          using SimulatorType = int;
          using ConfigurationType = int;
          using TimeType = detail::Signatures::time_type_result<ConformingType>;

          /// [expected_functions]
          using initialize_return_type =
              decltype(std::declval<ConformingType&>().initialize(
                  std::declval<SimulatorType&>()));
          using resolve_return_type =
              decltype(std::declval<ConformingType&>().resolve(
                  std::declval<SimulatorType&>(), std::declval<TimeType>()));
          static_assert(std::is_same_v<initialize_return_type, void>,
                        R"error(
Not a conforming Environment, doesn't properly implement function
`
template <typename Simulator>
void initialize(Simulator& simulator){
/*implementation defined*/
}
`
)error");

          static_assert(std::is_same_v<resolve_return_type, void>,
                        R"error(
Not a conforming Environment, doesn't properly implement function
`
template <typename Simulator>
void resolve(Simulator& simulator, Time time_v){
  /*implementation defined*/
}
`
)error");
          /// [expected_functions]

          /// [expected_free_functions]
          static_assert(::tt::is_detected_v<detail::Signatures::reset_result,
                                            ConformingType>,
                        R"error(
Not a conforming Environment, doesn't have a free-function
`void reset(ConformingType&, Time time_v){
  /*implementation defined*/
}`
)error");

          static_assert(::tt::is_detected_v<
                            detail::Signatures::initialize_parameters_result,
                            ConformingType, ConfigurationType>,
                        R"error(
Not a conforming Environment, doesn't have a free-function
`
template <typename Configuration>
void initialize_parameters(ConformingType&, Configuration const&){
/*implementation defined*/
}
`
)error");
          /// [expected_free_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
    }  // namespace protocols

  }  // namespace environments

}  // namespace elastica
