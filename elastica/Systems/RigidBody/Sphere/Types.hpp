//==============================================================================
/*!
//  \file
//  \brief All sphere component types
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

namespace elastica {

  namespace rigid_body {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename RBT, typename ComputationalBlock>
    class WithSphereGeometry;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename Traits, typename ComputationalBlock, typename Radius>
    inline decltype(auto) compute_volume(
        WithSphereGeometry<Traits, ComputationalBlock> const&,
        Radius const&) noexcept;
    template <typename Traits, typename ComputationalBlock, typename Radius>
    inline decltype(auto) compute_unscaled_moment_of_inertia(
        WithSphereGeometry<Traits, ComputationalBlock> const&,
        Radius const& radius) noexcept;
    /*! \endcond */
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica
