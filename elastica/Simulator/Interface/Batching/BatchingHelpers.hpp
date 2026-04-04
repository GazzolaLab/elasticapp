//==============================================================================
/*!
//  \file
//  \brief Common policies for batching objects in \elastica modules
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
#include <cstddef>

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"

namespace elastica {

  namespace interface {

    namespace detail {

      //**Type definitions****************************************************
      //! Definition of operator for constraining primitives
      template <typename Policy, class F>
      struct BatchingOperation;

      //! Sequential batching
      template <typename F>
      using BatchingOperationForSequentialExecution =
          BatchingOperation<::elastica::configuration::sequential_policy, F>;
      //! Parallel batching
      template <typename F>
      using BatchingOperationForParallelExecution =
          BatchingOperation<::elastica::configuration::parallel_policy, F>;
      //**********************************************************************

      //**********************************************************************
      /*!\brief Default operation for deciding the number of batches
       */
      struct DefaultDecisionForNumberOfBatches {
        std::size_t operator()(std::size_t const n_dofs,
                               std::size_t const n_threads) const {
          return n_dofs / n_threads;
        };
      };
      //**********************************************************************

      //**Type definitions****************************************************
      using DefaultDecision = DefaultDecisionForNumberOfBatches;
      using DefaultBatchingOperationForSequentialExecution =
          BatchingOperationForSequentialExecution<DefaultDecision>;
      using DefaultBatchingOperationForParallelExecution =
          BatchingOperationForParallelExecution<DefaultDecision>;
      //**********************************************************************

      //**********************************************************************
      /*!\brief Batch primitives for sequential execution
       */
      template <class F>
      struct BatchingOperation<::elastica::configuration::sequential_policy,
                               F> {
        //********************************************************************
        /*!\brief Policy constructor.
         */
        BatchingOperation(const std::size_t /*threadsize*/,
                          const std::size_t /*threshold*/, F const& /*meta*/) {}
        //********************************************************************

        //********************************************************************
        /*!\brief Returns the number of batches
         *
         * \param n_dofs The number of total constraints
         */
        inline auto operator()(std::size_t /*n_dofs*/) const noexcept
            -> std::size_t {
          return 1UL;
        }
        //********************************************************************
      };
      //**********************************************************************

      //**********************************************************************
      /*!\brief Batch primitives for parallel execution
       */
      template <class F>
      struct BatchingOperation<::elastica::configuration::parallel_policy, F> {
        //********************************************************************
        /*!\brief Policy constructor.
         */
        BatchingOperation(const std::size_t threadsize,
                          const std::size_t /*threshold*/, F f)
            : n_threads(threadsize), func(std::move(f)) {}
        //********************************************************************

        //********************************************************************
        /*!\brief Returns the number of batches
         *
         * \param n_dofs The number of total constraints
         */
        inline auto operator()(std::size_t n_dofs) const noexcept
            -> std::size_t {
          return func(n_dofs, n_threads);
        }
        //********************************************************************

        //**Member variables**************************************************
        //! Handle to the Module
        std::size_t const n_threads;
        //! The decision functor
        F func;
        //********************************************************************
      };
      //**********************************************************************

      //**********************************************************************
      /*!\brief Batch primitives for parallel execution
       */
      template <class F>
      struct BatchingOperation<::elastica::configuration::hybrid_policy, F> {
        //********************************************************************
        /*!\brief Policy constructor.
         */
        BatchingOperation(const std::size_t threadsize,
                          const std::size_t threshold, F f)
            : batch_for_sequence_(threadsize, threshold,
                                  f),  // f doesn't matter here
              batch_for_parallel_(threadsize, threshold, std::move(f)),
              parallelism_threshold_(threshold) {}
        //********************************************************************

        //********************************************************************
        /*!\brief Returns the number of batches
         *
         * \param n_dofs The number of total constraints
         */
        inline auto operator()(std::size_t n_dofs) const noexcept
            -> std::size_t {
          if (n_dofs < parallelism_threshold_)
            return batch_for_sequence_(n_dofs);
          else
            return batch_for_parallel_(n_dofs);
        }
        //********************************************************************

        //**Member variables**************************************************
        //! Serial forcing functor
        BatchingOperationForSequentialExecution<F> batch_for_sequence_;
        //! Parallel forcing functor
        BatchingOperationForParallelExecution<F> batch_for_parallel_;
        //! Threshold to switch between serial and parallel execution
        const std::size_t parallelism_threshold_;
        //********************************************************************
      };
      //**********************************************************************

    }  // namespace detail

  }  // namespace interface

}  // namespace elastica
