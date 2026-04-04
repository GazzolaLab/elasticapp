//==============================================================================
/*!
//  \file
//  \brief All rigid body component types
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
    class WithRigidBodyGeometry;

    template <typename RBT, typename ComputationalBlock>
    class WithRigidBodyKinematics;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */

    template <typename Traits, typename ComputationalBlock>
    void update_acceleration(
        WithRigidBodyKinematics<Traits, ComputationalBlock>&
            block_like) noexcept;
    /*! \endcond */
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica