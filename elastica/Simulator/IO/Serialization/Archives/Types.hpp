//==============================================================================
/*!
//  \file
//  \brief All archive types belonging to the Serialization component
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

// not a recursive include, the pragma mechanism takes care
#include "Simulator/IO/Serialization/Types.hpp"

namespace elastica {

  namespace io {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of serialization archive types
    //
    ////////////////////////////////////////////////////////////////////////////

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    class BinaryOutputArchive;
    class BinaryInputArchive;
    /*! \endcond */
    //**************************************************************************

    namespace detail {
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // Forward declaration of internal functions
      template <typename T>
      void save_internal(BinaryOutputArchive&, T const&);
      template <typename T>
      void save_internal(BinaryOutputArchive&, BinaryData<T> const&);
      template <typename T>
      void load_internal(BinaryInputArchive&, T&);
      template <typename T>
      void load_internal(BinaryInputArchive&, BinaryData<T>&);
      /*! \endcond */
      //************************************************************************
    }  // namespace detail

  }  // namespace io

}  // namespace elastica
