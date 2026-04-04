//==============================================================================
/*!
//  \file
//  \brief Tags for adding explicit damping to an Elasticity component
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
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

    //**************************************************************************
    /*!\brief Tag indicating rate of damping for the translational component of
     * of cosserat rod dynamics. Used in adding a damping force with an usual
     * form of
     * \f[ F_\mathrm{damp} \propto m \cdot \mathrm{rate} \cdot v \f]
     * \ingroup system_tags
     */
    struct ForceDampingRate {};
    //**************************************************************************

    //**************************************************************************
    /*!\brief Tag indicating rate of damping for the rotational component of
     * of cosserat rod dynamics. Used in adding a damping torque with an usual
     * form of
     * \f[ T_\mathrm{damp} \propto m \cdot \mathrm{rate} \cdot \omega \f]
     * \ingroup system_tags
     */
    struct TorqueDampingRate {};
    //**************************************************************************

  }  // namespace tags

}  // namespace elastica
