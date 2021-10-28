//==============================================================================
/*!
//  \file
//  \brief Tags for describing cross sectional properties of a Cosserat Rod
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

    struct ElementDimension{};
    struct ElementVolume{};
    // so3 group

    //**************************************************************************
    /*!\brief Tag indicating mass second moment of inertia
     * \ingroup systems
     */
    struct MassSecondMomentOfInertia{};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Tag indicating inverse mass second moment of inertia
     * equivalent to inverse of mass second moment of inertia.
     * \ingroup systems
     */
    struct InvMassSecondMomentOfInertia{};
    //**************************************************************************


  }  // namespace tags

}  // namespace elastica
