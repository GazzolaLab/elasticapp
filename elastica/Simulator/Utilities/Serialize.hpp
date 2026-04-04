//==============================================================================
/*!
//  \file
//  \brief Serialization support for simulator utilities
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

/// base serialization
#include "Simulator/IO/Serialization/Custom/TaggedTuple.hpp"
#include "Simulator/IO/Serialization/Custom/Timer.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
///
#include "Simulator/Utilities/Timers.hpp"
///

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for Timers
     * \ingroup domain
     */
    template <typename Tags, typename TP>
    struct Serialize<::elastica::Timers<Tags, TP>> {
      using type = ::elastica::Timers<Tags, TP>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& timers) {
        ar(make_nvp("sections", timers.data()));
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& timers) {
        ar(make_nvp("sections", timers.data()));
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
