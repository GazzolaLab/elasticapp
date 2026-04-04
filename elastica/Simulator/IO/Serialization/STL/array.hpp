//==============================================================================
/*!
//  \file
//  \brief Support for types found in \<array\>
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

#include <array>
#include <type_traits>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::arraytor
    // \ingroup stl_support
    */
    template <typename T, std::size_t N>
    struct Serialize<std::array<T, N>> {
      using type = std::array<T, N>;

      // specialize for arithmetic types and non-arithmetic types
      // helpful tags for specialization
      struct arithmetic_tag {};
      struct non_arithmetic_tag {};

      using specialized_type =
          std::conditional_t<std::is_arithmetic<T>::value, arithmetic_tag,
                             non_arithmetic_tag>;

      static inline void load_helper(BinaryInputArchive& ar, type& array,
                                     arithmetic_tag /* meta */) {
        ar(binary_data(array.data(), sizeof(type)));
      }

      static inline void save_helper(BinaryOutputArchive& ar, type const& array,
                                     arithmetic_tag /* meta */) {
        ar(binary_data(array.data(), sizeof(type)));
      }

      template <typename InArchive>
      static inline void load_helper(InArchive& ar, type& array,
                                     arithmetic_tag /* meta */) {
        load_helper(ar, array, non_arithmetic_tag{});
      }

      template <typename InArchive>
      static inline void save_helper(InArchive& ar, type const& array,
                                     arithmetic_tag /* meta */) {
        save_helper(ar, array, non_arithmetic_tag{});
      }

      template <typename InArchive>
      static inline void load_helper(InArchive& ar, type& array,
                                     non_arithmetic_tag /* meta */) {
        for (auto&& v : array)
          ar(v);
      }

      template <typename OutArchive>
      static inline void save_helper(OutArchive& ar, type const& array,
                                     non_arithmetic_tag /* meta */) {
        for (auto&& v : array)
          ar(v);
      }

      template <typename InArchive>
      static inline void load(InArchive& ar, type& array) {
        load_helper(ar, array, specialized_type{});
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& array) {
        save_helper(ar, array, specialized_type{});
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
