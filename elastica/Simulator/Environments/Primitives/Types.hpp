//==============================================================================
/*!
//  \file
//  \brief Types of primitives
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

#include "Simulator/Environments/Primitives/detail/Types.hpp"

namespace elastica {

  namespace collision {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of elastica::collision::primitive types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    // Forward declarations of types
    class RigidBodyPrimitive;
    class GeomPrimitive;
    class SpherePrimitive;
    class PlanePrimitive;
    class HalfSpacePrimitive;
    // class ApproximateCylinderPrimitive;
    class CylinderPrimitive;
    class CapsulePrimitive;
    class CapsuleFromRodPrimitive;
    template <typename T>
    class ImposedPrimitiveAdapter;

    using GeomID = GeomPrimitive*;
    using ConstGeomID = const GeomPrimitive*;

    using BodyID = RigidBodyPrimitive*;
    using ConstBodyID = const RigidBodyPrimitive*;
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica