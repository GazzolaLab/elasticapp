//==============================================================================
/*!
//  \file
//  \brief Collision kernel execution policy
//
//  Copyright (C) 2020-2022 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2022 MattiaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <algorithm>
#include <utility>
///
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
///
#include "Simulator/Environments/CollisionSystem/Policy/ExecutionPolicy/Types.hpp"
//
#include "Simulator/Environments/CollisionSystem/Policy/ExecutionPolicy/ForEach.hpp"
#include "Simulator/Environments/CollisionSystem/Policy/ExecutionPolicy/ForEachType.hpp"
#include "Simulator/Environments/CollisionSystem/Policy/ExecutionPolicy/PairwiseForEach.hpp"
///
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*!\brief Policy class for execution of collision kernels
     * \ingroup collision_system
     *
     * \tparam ExecutionPolicyForEach_ The execution policy to be
     * followed over while iterating over a collection, see @ref execution
     * \tparam ExecutionPolicyOverPairs_ The execution policy to be
     * followed over while iterating over a collection of pairs, see @ref
     * execution
     */
    template <typename ForEachExecutionPolicy_,
              typename OverTypesExecutionPolicy>
    struct KernelExecutionPolicy
        : public detail::ForEachOp<ForEachExecutionPolicy_>,
          public detail::ForEachTypeOp<OverTypesExecutionPolicy>,
          public detail::OverPairsOp<OverTypesExecutionPolicy> {
     protected:
      //**Type definitions******************************************************
      //! Internal ordering to find which is the execution policy
      using Ordered = tmpl::list<::elastica::configuration::sequential_policy,
                                 ::elastica::configuration::parallel_policy,
                                 ::elastica::configuration::hybrid_policy>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Execution policy for configuring the collision module
      using ExecutionPolicy = tmpl::at<
          Ordered,
          tmpl::max<tmpl::index_of<Ordered, ForEachExecutionPolicy_>,
                    tmpl::index_of<Ordered, OverTypesExecutionPolicy>>>;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica