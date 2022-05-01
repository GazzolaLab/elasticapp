//==============================================================================
/*!
//  \file
//  \brief Types of primitives
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
    // class ApproximateCylinderPrimitive;
    class CylinderPrimitive;
    class CapsulePrimitive;
    class CapsuleFromRodPrimitive;
    template <typename T>
    class ImposedPrimitiveAdapter;

    /* Do not expose to clients
    using ImposedSphere = ImposedPrimitiveAdapter<SpherePrimitive>;
    using ImposedCapsule = ImposedPrimitiveAdapter<CapsulePrimitive>;
    using ImposedPlane = ImposedPrimitiveAdapter<PlanePrimitive>;
    using ImposedCylinder = ImposedPrimitiveAdapter<CylinderPrimitive>;
    */

    using GeomID = GeomPrimitive*;
    using ConstGeomID = const GeomPrimitive*;

    using BodyID = RigidBodyPrimitive*;
    using ConstBodyID = const RigidBodyPrimitive*;
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica