//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Serialization component
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

  namespace io {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of serialization types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename T>
    struct Serialize;

    template <typename T>
    struct SerializeWizard;

    template <class ArchiveType>
    class InputArchive;

    template <class ArchiveType>
    class OutputArchive;

    template <typename, typename>
    class MapItem;

    template <typename T>
    class NameValuePair;

    template <typename T>
    class SizeTag;

    template <typename T>
    struct BinaryData;
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica

//******************************************************************************
// Post Includes
//******************************************************************************
#include "Simulator/IO/Serialization/Archives/Types.hpp"
