//==============================================================================
/*!
//  \file
//  \brief Batching policies for the \elastica Constraints module
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
///
#include "Simulator/Interface/Batching/BatchingHelpers.hpp"
#include "Simulator/Interface/Connections/Types.hpp"
///
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace constraints {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Batching policy for Constraints module
       * \ingroup interface
       *
       * This class represents a decision policy to batch constraints
       * for execution policies span a spectrum for purely sequential execution
       * to parallel, asynchronous execution.
       *
       * \tparam ConstraintsTraits Options for configuration of the Constraints
       * Module
       *
       * \see elastica::interface::Constraints
       */
      template <class ConstraintsTraits>
      class BatchingPolicy {
       protected:
        //**Type definitions****************************************************

        //**********************************************************************
        /*!\brief Batch primitives for parallel execution
         */
        template <typename Policy, class F>
        struct BatchingOperation
            : public ::elastica::interface::detail::BatchingOperation<Policy,
                                                                      F> {
          using P = ::elastica::interface::detail::BatchingOperation<Policy, F>;
          using P::operator();

          explicit BatchingOperation(ConstraintsTraits const& traits)
              : P(traits.n_threads(), 0UL /*unused parallel threshold*/, F()) {}
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Batch primitives for hybrid execution, need to specialize
         * because hybrid policy is specialized
         */
        template <class F>
        struct BatchingOperation<::elastica::configuration::hybrid_policy, F>
            : public ::elastica::interface::detail::BatchingOperation<
                  ::elastica::configuration::hybrid_policy, F> {
          using P = ::elastica::interface::detail::BatchingOperation<
              ::elastica::configuration::hybrid_policy, F>;
          using P::operator();

          explicit BatchingOperation(ConstraintsTraits const& traits)
              : P(traits.n_threads(),
                  traits.constraints_parallelism_threshold(), F()) {}
        };
        //**********************************************************************

       protected:
        //**Algorithms**********************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Algorithms for constraints*/
        //@{
        struct ConstraintsBatcher {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = BatchingOperation<
              typename ConstraintsTraits::ConstraintsExecutionPolicy,
              // TODO : another F from Traits can be filled, for now leave empty
              ::elastica::interface::detail::DefaultDecision>;
          //********************************************************************
        };
        //@}
        /*! \endcond */
        //**********************************************************************

       public:
        //**Batching algorithm**************************************************
        //! Algorithm for batching
        using BatchingAlgorithm = tmpl::list<ConstraintsBatcher>;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace constraints

  }  // namespace interface

}  // namespace elastica