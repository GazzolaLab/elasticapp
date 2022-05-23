//==============================================================================
/*!
//  \file
//  \brief All out-of-the-box imposed primitive types in \elastica
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
#include "Simulator/Environments/Primitives/Types.hpp"

namespace elastica {

  namespace collision {

    //**Type definitions********************************************************
    //! Type of imposed sphere
    using ImposedSpherePrimitive = ImposedPrimitiveAdapter<SpherePrimitive>;
    //! Type of imposed capsule
    using ImposedCapsulePrimitive = ImposedPrimitiveAdapter<CapsulePrimitive>;
    //! Type of imposed plane
    using ImposedPlanePrimitive = ImposedPrimitiveAdapter<PlanePrimitive>;
    //! Type of imposed cylinder
    using ImposedCylinderPrimitive = ImposedPrimitiveAdapter<CylinderPrimitive>;
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
