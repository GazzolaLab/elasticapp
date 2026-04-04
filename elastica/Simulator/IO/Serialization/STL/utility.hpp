//==============================================================================
/*!
//  \file
//  \brief Support for types found in \<pair\>
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

#include <utility>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::pair
    // \ingroup stl_support
    */
    template <typename T1, typename T2>
    struct Serialize<std::pair<T1, T2>> {
      using type = std::pair<T1, T2>;

      template <typename Archive>
      static inline void load(Archive& ar, type& pair) {
        ar(make_nvp<Archive>("first", pair.first),
           make_nvp<Archive>("second", pair.second));
      }

      template <typename Archive>
      static inline void save(Archive& ar, type const& pair) {
        ar(make_nvp<Archive>("first", pair.first),
           make_nvp<Archive>("second", pair.second));
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
