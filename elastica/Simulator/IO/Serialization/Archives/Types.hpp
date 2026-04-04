//==============================================================================
/*!
//  \file
//  \brief All archive types belonging to the Serialization component
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
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
    class YAMLOutputArchive;
    class YAMLInputArchive;
    /*! \endcond */
    //**************************************************************************

    namespace detail {
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // Forward declaration of internal functions
      template <typename T>
      void save_internal(BinaryOutputArchive&, T const&);
      template <typename T>
      void load_internal(BinaryInputArchive&, T&);

      template <typename T>
      void save_internal(YAMLOutputArchive&, T const&);
      template <typename T>
      void load_internal(YAMLInputArchive&, T&);
      /*! \endcond */
      //************************************************************************
    }  // namespace detail

  }  // namespace io

}  // namespace elastica
