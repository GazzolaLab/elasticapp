//==============================================================================
/*!
//  \file
//  \brief Support for types found in \<vector\>
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

#include <type_traits>
#include <vector>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::vector
    // \ingroup stl_support
    */
    template <typename T, class Alloc>
    struct Serialize<std::vector<T, Alloc>> {
      using type = std::vector<T, Alloc>;

      // specialize for boolean types, arithmetic types and non-arithmetic types
      // helpful tags for specialization
      struct bool_tag {};
      struct arithmetic_tag {};
      struct non_arithmetic_tag {};

      using specialized_type = std::conditional_t<
          cpp17::is_same_v<T, bool>, bool_tag,
          std::conditional_t<std::is_arithmetic<T>::value, arithmetic_tag,
                             non_arithmetic_tag>>;

      template <typename InArchive>
      static inline void load_helper(InArchive& ar, type& vec,
                                     bool_tag /* meta */) {
        for (auto v : vec) {
          bool b;
          ar(b);
          v = b;
        }
      }

      static inline void load_helper(BinaryInputArchive& ar, type& vec,
                                     arithmetic_tag /* meta */) {
        ar(binary_data(vec.data(),
                       static_cast<std::size_t>(vec.size()) * sizeof(T)));
      }

      template <typename InArchive>
      static inline void load_helper(InArchive& ar, type& vec,
                                     arithmetic_tag /* meta */) {
        // non-binary archives load as objects
        load_helper(ar, vec, non_arithmetic_tag{});
      }

      template <typename InArchive>
      static inline void load_helper(InArchive& ar, type& vec,
                                     non_arithmetic_tag /* meta */) {
        for (auto&& v : vec)
          ar(v);
      }

      template <typename InArchive>
      static inline void load(InArchive& ar, type& vec) {
        std::size_t size;
        ar(make_size_tag(size));
        vec.resize(static_cast<std::size_t>(size));
        // wish for if constexpr
        load_helper(ar, vec, specialized_type{});
      }

      template <typename OutArchive>
      static inline void save_helper(OutArchive& ar, type const& vec,
                                     bool_tag /* meta */) {
        // boolean vectors are bitsets per standard, so type cast
        for (const auto v : vec) {
          ar(static_cast<bool>(v));
        }
      }

      static inline void save_helper(BinaryOutputArchive& ar, type const& vec,
                                     arithmetic_tag /* meta */) {
        ar(binary_data(vec.data(), vec.size() * sizeof(T)));
      }

      template <typename OutArchive>
      static inline void save_helper(OutArchive& ar, type const& vec,
                                     arithmetic_tag /* meta */) {
        save_helper(ar, vec, non_arithmetic_tag{});
      }

      template <typename OutArchive>
      static inline void save_helper(OutArchive& ar, type const& vec,
                                     non_arithmetic_tag /* meta */) {
        for (auto&& v : vec)
          ar(v);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& vec) {
        ar(make_size_tag(static_cast<std::size_t>(vec.size())));
        // wish for if constexpr
        save_helper(ar, vec, specialized_type{});
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
