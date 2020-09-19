//==============================================================================
/*!
//  \file
//  \brief Transfer types
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


    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Forward declarations of transfer types
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    // The main classes
    template <typename SystemTag>
    struct TransferTraits;
    template <typename SystemTag>
    struct Transfer;
    template <typename SystemTag>
    struct TransferWizard;

    // Details and tags
    namespace tags {
      struct from_t;
      struct to_t;
    }  // namespace tags
    /*! \endcond */
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
