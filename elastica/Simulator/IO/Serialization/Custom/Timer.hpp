//==============================================================================
/*!
//  \file
//  \brief Serialization support for types found in \<Timer.h\>
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

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/Timing/Timer.h"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for Timer
    // \ingroup custom_serialize
    */
    template <typename TP>
    struct Serialize<elastica::timing::Timer<TP>> {
      using type = elastica::timing::Timer<TP>;

      template <typename Archive, typename T>
      static inline void load_save(Archive& ar, T&& timer) {
        ar(make_nvp<Archive>("total", timer.get_total()));
        ar(make_nvp<Archive>("counter", timer.get_counter()));
        // average can be inferred, but we serialize it in readable archives
        auto average = timer.average();
        ar(make_nvp<Archive>("average", average));
        ar(make_nvp<Archive>("min", timer.get_min()));
        ar(make_nvp<Archive>("max", timer.get_max()));
        ar(make_nvp<Archive>("last", timer.get_last()));
      }

      template <typename InArchive>
      static inline void load(InArchive& ar, type& timer) {
        load_save(ar, timer);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& timer) {
        load_save(ar, timer);
      }
    };

    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
