//==============================================================================
/*!
//  \file
//  \brief Access tags of @ref cosserat_rod within \elastica
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
#include "Systems/CosseratRods/Components/Tags.hpp"
#include "Systems/common/Tags.hpp"

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
    struct ForceDamping;
    struct TorqueDamping;
    */

  }  // namespace tags

}  // namespace elastica