//==============================================================================
/*!
//  \file
//  \brief Helpers to initialize Rigidbody components
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

#include "Systems/common/Initialization.hpp"
//
#include "Systems/RigidBody/RigidBodyPlugin.hpp"

namespace elastica {

  namespace rigid_body {

    //**************************************************************************
    /*!\brief Component initializer for a Rigid-body component
     * \ingroup rigid_body
     *
     * \tparam Variables Typelist of `BlockVariables` to initialize.
     * \param block_like Rigid-body block-slice.
     * \param initializers Rigid-body block-initializer(s).
     */
    template <typename Variables, typename RBT,
              template <typename /*CRT*/, typename /* ComputationalBlock */>
              class... Components,
              // typename Initializer>
              typename... Initializers>
    inline void initialize_component(
        ::blocks::BlockSlice<RigidBodyPlugin<RBT, ::blocks::BlockSlice,
                                             Components...>>& block_like,
        ::blocks::BlockInitializer<
            RigidBodyPlugin<RBT, ::blocks::Block, Components...>,
            Initializers...>
            initializers) {
      ::elastica::detail::initialize_component<Variables>(
          block_like, std::move(initializers));
    }
    //**************************************************************************

    // FIXME : https://github.com/tp5uiuc/elasticapp/issues/460
    // Don't use initialize on model directly, so remove this function.
    //**************************************************************************
    /*!\brief Component initializer for a Rigid-body component
     * \ingroup rigid_body
     *
     * \tparam Variables Typelist of `BlockVariables` to initialize.
     * \param block_like Rigid-body block.
     * \param initializers Rigid-body block-initializer(s).
     */
    template <typename Variables, typename RBT,
              template <typename /*CRT*/, typename /* ComputationalBlock */>
              class... Components,
              // typename Initializer>
              typename... Initializers>
    void initialize_component(
        ::blocks::Block<RigidBodyPlugin<RBT, ::blocks::Block,
                                        Components...>>& block_like,
        ::blocks::BlockInitializer<
            RigidBodyPlugin<RBT, ::blocks::Block, Components...>,
            Initializers...>
            initializers) {
      ::elastica::detail::initialize_component<Variables>(
          block_like, std::move(initializers));
    }
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica
