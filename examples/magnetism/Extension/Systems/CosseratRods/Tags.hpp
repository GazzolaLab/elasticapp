//==============================================================================
/*!
//  \file
//  \brief Access tags of @ref cosserat_rod within \elastica
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
#include <elastica/Systems/CosseratRods/Tags.hpp>

#include "Extension/Systems/CosseratRods/Components/Tags.hpp"

namespace elastica {

  namespace tags {

    //==========================================================================
    //
    //  TAG DEFINITIONS
    //
    //==========================================================================

    // 1D Geometry types
    /*
    struct ReferenceElementLength;
    struct ReferenceCurvature;
    struct ReferenceVoronoiLength;
    struct ElementLength;
    struct ElementDilatation;
    struct VoronoiLength;
    struct VoronoiDilatation;
    struct Curvature;
    struct Tangent;
    struct ShearStretchStrain;
    */

    // 2D Geometry
    /*
    struct Dimension;
    struct Volume;

    // Elasticity
    struct InternalLoads;
    struct InternalTorques;

    // Linear hyper-elasticity
    struct BendingTwistRigidityMatrix;
    struct ShearStretchRigidityMatrixCollectionTag;

    // Linear visco-elasticity
    struct ForceDampingCoefficient;
    struct TorqueDampingCoefficient;
    */

  }  // namespace tags

}  // namespace elastica
