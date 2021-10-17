//==============================================================================
/*!
//  \file
//  \brief Batching policies for the \elastica Forcing module
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
///
#include "Simulator/Interface/Batching/BatchingHelpers.hpp"
#include "Simulator/Interface/Connections/Types.hpp"
///
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace forcing {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Batching policy for Forcing module
       * \ingroup interface
       *
       * This class represents a decision policy to batch forcing
       * for execution policies span a spectrum for purely sequential execution
       * to parallel, asynchronous execution.
       *
       * \tparam ForcingTraits Options for configuration of the Forcing
       * Module
       *
       * \see elastica::interface::Forcing
       */
      template <class ForcingTraits>
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

          explicit BatchingOperation(ForcingTraits const& traits)
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

          explicit BatchingOperation(ForcingTraits const& traits)
              : P(traits.n_threads(),
                  traits.forcing_parallelism_threshold(), F()) {}
        };
        //**********************************************************************

       protected:
        //**Algorithms**********************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Algorithms for forcing*/
        //@{
        struct ForcingBatcher {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = BatchingOperation<
              typename ForcingTraits::ForcingExecutionPolicy,
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
        using BatchingAlgorithm = tmpl::list<ForcingBatcher>;
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace forcing

  }  // namespace interface

}  // namespace elastica
