//==============================================================================
/*!
//  \file
//  \brief Forcing configuration protocol
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
#include <utility>  // declval

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Utilities/TMPL.hpp"
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
      /*!\brief Forcing configuration protocol.
       * \ingroup configuration_protocols
       *
       * Class to enforce adherence to a Forcing configuration protocol. Any
       * valid Forcing configuration class within the \elastica library
       * should (publicly) inherit from this class to indicate it qualifies as a
       * Forcing configuration. Only in case a class is derived publicly from
       * this base class, the tt::conforms_to and tt::assert_conforms_to type
       * traits recognizes the class as semantically valid.
       *
       * Requires that a conforming type `ConformingType` has these nested types
       * \snippet this expected_types
       * and these member functions,
       * \snippet this expected_functions
       *
       * \example
       * ::elastica::configuration::ForcingConfiguration shows a minimal
       * example of conformance to this protocol
       */
      struct ForcingConfiguration {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup protocols
        */
        template <typename ConformingType>
        struct test {
         public:
          /// [expected_types]
          //**Type definitions**************************************************
          //! Type of execution policy
          using ExecutionPolicy =
              typename ConformingType::ForcingExecutionPolicy;
          // one of sequential or primitive
          static_assert(
              tmpl::found<
                  tmpl::list<sequential_policy, hybrid_policy, parallel_policy>,
                  std::is_same<tmpl::_1, ExecutionPolicy>>::value,
              "ExecutionPolicy should either be sequential, "
              "parallel or hybrid");
          //********************************************************************
          /// [expected_types]

          /// [expected_functions]
          using size = std::size_t;
          template <typename Type>
          using return_type = decltype(std::declval<Type const&>()
                                           .forcing_parallelism_threshold());
          using parallelism_threshold_return_type = std::conditional_t<
              std::is_same_v<ExecutionPolicy, hybrid_policy>,
              ::tt::detected_t<return_type, ConformingType>, size>;

          static_assert(
              std::is_same_v<size, parallelism_threshold_return_type>,
              R"error(
Not a conforming hybrid Forcing configuration, doesn't properly implement function
`std::size_t forcing_parallelism_threshold() const`)error");
          /// [expected_functions]
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace configuration

}  // namespace elastica
