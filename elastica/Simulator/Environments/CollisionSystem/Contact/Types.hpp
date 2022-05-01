//==============================================================================
/*!
//  \file
//  \brief Types of contacts submodule
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

  namespace collision {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    class Contact;
    class ContactBase;
    template <typename C>
    class ContactTrait;
    enum class ContactType;

    //**************************************************************************
    /*!\brief Handle to a contact between two rigid bodies.
     * \ingroup collision_system
     */
    // using ContactID = Contact*;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Handle to a constant contact between two rigid bodies.
     * \ingroup collision_system
     */
    // using ConstContactID = const Contact*;
    //**************************************************************************

    /*! \endcond */
    //**************************************************************************
  }  // namespace collision

}  // namespace elastica