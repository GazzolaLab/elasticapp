//==============================================================================
/*!
//  \file
//  \brief Tags for adding explicit damping to an Elasticity component
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

    // Damping such that F \prop v and \tau \prop \omega
    struct ForceDampingCoefficient{};
    struct TorqueDampingCoefficient{};

  }  // namespace tags

}  // namespace elastica
