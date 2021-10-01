//==============================================================================
/*!
//  \file
//  \brief Tags for full description of a Cosserat Rod geometry
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  //============================================================================
  //
  //  TAG DEFINITIONS
  //
  //============================================================================
  namespace tags {

    // 1D Geometry types
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

  }  // namespace tags

}  // namespace elastica