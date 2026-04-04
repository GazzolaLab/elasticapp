//==============================================================================
/*!
//  \file
//  \brief Support for types found in \<map\>
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

// Reused from cereal : https://uscilab.github.io/cereal/index.html
// Distributed under the BSD License.
// See https://github.com/USCiLab/cereal/blob/master/LICENSE for details.
#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Simulator/IO/Serialization/STL/associative_containers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
//
#include <map>

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::unordered_map
    // \ingroup stl_support
    */
    template <typename... Args>
    struct Serialize<std::map<Args...>> {
      using type = std::map<Args...>;

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& map) {
        save_map(ar, map);
      }

      template <typename OutArchive>
      static inline void load(OutArchive& ar, type& map) {
        load_map(ar, map);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
