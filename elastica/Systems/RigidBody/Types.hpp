//==============================================================================
/*!
//  \file
//  \brief All types belonging to @ref rigid_body
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

#include "Systems/Block/Types.hpp"
#include "Systems/RigidBody/Components/Types.hpp"
#include "Systems/RigidBody/Sphere/Types.hpp"

namespace elastica {

  namespace rigid_body {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of rigid body types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename /*DataType*/, typename /*AllocatorTraits*/>
    struct RigidBodyTraits;

    struct DefaultRigidBodyTraits;

    template <typename /*CRT*/,
              template <typename /*Plugin*/> class /*ComputationalBlock*/,
              template <typename /*CRT*/, typename /*Instantiated Block*/>
              class... /*Components*/>
    class RigidBodyPlugin;

    template <typename /*CRT*/,
              template <typename /*Plugin*/> class /*ComputationalBlock*/,
              typename /*Tag*/,
              template <typename /*CRT*/, typename /*Instantiated Block*/>
              class... /*Components*/>
    class TaggedRigidBodyPlugin;

    /*! \endcond */
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica