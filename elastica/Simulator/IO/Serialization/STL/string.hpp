//==============================================================================
/*!
//  \file
//  \brief Support for types found in \<string\>
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

// Reused from cereal : https://uscilab.github.io/cereal/index.html
// Distributed under the BSD License.
// See https://github.com/USCiLab/cereal/blob/master/LICENSE for details.

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <string>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::basic_string
    // \ingroup stl_support
    */
    template <class CharT, class Traits, class Alloc>
    struct Serialize<std::basic_string<CharT, Traits, Alloc>> {
      using type = std::basic_string<CharT, Traits, Alloc>;

      static inline void load(BinaryInputArchive& ar, type& str) {
        std::size_t size;
        ar(make_size_tag(size));
        str.resize(static_cast<std::size_t>(size));
        ar(binary_data(const_cast<CharT*>(str.data()),
                       static_cast<std::size_t>(size) * sizeof(CharT)));
      }

      template <typename InArchive>
      static inline void load(InArchive& ar, type& str) {
        using detail::load_internal;
        load_internal(ar, str);
      }

      static inline void save(BinaryOutputArchive& ar, type const& str) {
        ar(make_size_tag(static_cast<std::size_t>(str.size())));
        ar(binary_data(str.data(), str.size() * sizeof(CharT)));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& str) {
        using detail::save_internal;
        save_internal(ar, str);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica