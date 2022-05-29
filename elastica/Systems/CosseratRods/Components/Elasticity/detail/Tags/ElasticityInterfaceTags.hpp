//==============================================================================
/*!
//  \file
//  \brief Tags for interface to an Elasticity component of Cosserat Rods
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

    struct InternalLoads {};
    struct InternalStress {};
    struct InternalTorques {};
    struct InternalCouple {};

    struct _DummyElementVector {};
    struct _DummyElementVector2 {};
    struct _DummyVoronoiVector {};
    struct _DummyMatrix {};

  }  // namespace tags

}  // namespace elastica